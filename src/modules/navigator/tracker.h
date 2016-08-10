
#ifndef NAVIGATOR_TRACKER_H
#define NAVIGATOR_TRACKER_H

#include <limits.h>
#include <uORB/topics/home_position.h>
#include <uORB/topics/vehicle_local_position.h>


// Enables verbose debug messages by the flight path tracker
#define DEBUG_TRACKER


#ifdef DEBUG_TRACKER
#define TRACKER_DBG(...) PX4_INFO(__VA_ARGS__)
#else
#define TRACKER_DBG(...)
#endif


class Tracker
{
    friend class TrackerTest;
    
public:

    struct path_finding_context_t;

    // Informs the tracker about a new home position.
    void set_home(home_position_s *position) { set_home(position->x, position->y, position->z); }
    
    // Informs the tracker about a new current vehicle position.
    void update(vehicle_local_position_s *position) { if (position->xy_valid && position->z_valid) update(position->x, position->y, position->z); }
    
    // Pops the last position from the recent path. Returns false if the path is empty.
    bool pop_recent_path(double &lat, double &lon, float &alt);
    
    // Enables or disables tracking of the recent path.
    void set_recent_path_tracking_enabled(bool enabled) { recent_path_tracking_enabled = enabled; }

    // Initializes a path finding context starting at the current position.
    // The context must be viewed as an opaque value that can for instance be passed to the advance_return_path function.
    // Returns true on success.
    bool init_return_path(path_finding_context_t &context, float &x, float &y, float &z);

    // Fetches the next position on the shortest path back home (starting at, but excluding the position stored by the provided context).
    // The context is updated accordingly.
    // Returns true if the returned position is valid.
    bool advance_return_path(path_finding_context_t &context, float &x, float &y, float &z);

    // Checks if the provided context is currently close to the latest position that the tracker recorded.
    bool is_context_close_to_head(path_finding_context_t &context);

    // Checks if the provided context is currently close to the home position of the tracker.
    bool is_context_close_to_home(path_finding_context_t &context);

    // Checks if both of the provided contexts refer to the same position.
    bool is_same_pos(path_finding_context_t &context1, path_finding_context_t &context2);
    
    // Dumps the points in the recent path to the log output
    void dump_recent_path(void);
    
    // Dumps the content of the full flight graph to the log output
    void dump_graph(void);
    
    // Dumps the payload value of each node in the graph
    void dump_nodes(void);
    
    // Dumps the shortest path from the current position to the home position
    void dump_path_to_home(void);

    
private:
    
    /*** configuration ***/

    // Tracking accuracy in meters
	static constexpr float ACCURACY = 2;
	static constexpr float GRID_SIZE = ACCURACY / 2;
    // todo: grid size should equal GRID_SIZE
    
    // Number of positions that are retained in the recent path buffer.
    // This must be a multiple of 16 (?)
    static constexpr int RECENT_PATH_LENGTH = 64;
    
    // Number of small lines that can be stored in the full flight graph.
    // The actual number of position depends on how how well the path can be optimized.
    static constexpr int GRAPH_LENGTH = 256;

    // Number of indices to scan at each update to find intersections.
    // If the entire buffer is scanned, CPU usage may be too high.
    static constexpr int GRAPH_SEARCH_RANGE = GRAPH_LENGTH;

    // The maximum number of positions that should be recorded until the new positions are consolidated.
    // At the beginning and end of each consolidation pass, usually some optimization opportunities are missed.
    // For this reason, a larger number is preferred here.
    // The only drawback of a larger number should be a high workload once consolidation kicks in.
    static constexpr int MAX_CONSOLIDATION_DEPT = 64;
    
    // Number of intersections that can be stored.
    static constexpr int NODE_BUFFER_SIZE = 20;


    // Limitations and properties inherent to the graph representation (should not be changed)
    static constexpr int COMPACT_DELTA_MIN = -16;
    static constexpr int COMPACT_DELTA_MAX = 15;
    static constexpr int FAR_DELTA_MIN = -16384; // 0xC000
    static constexpr int FAR_DELTA_MAX = 16383; // 0x3FFF
    static constexpr int FAR_DELTA_SIZE = 3;
    static constexpr int MAX_INDEX = 0xFFFF;
    static constexpr int MAX_COEFFICIENT = 0x7FFF;
    static constexpr int MAX_DISTANCE = 0x7FFF;

#if GRAPH_LENGTH > MAX_INDEX
#error "GRAPH_LENGTH too large"
#endif


    /*** internal type definitions ***/

    typedef uint16_t delta_item_t;

    // sizeof(node_t) should be 12 bytes (verify this!)
    struct node_t {
        uint16_t index1;            // identifies the first line of the intersection
        uint16_t index2;            // identifies the second line of the intersection
        unsigned int coef1 : 15;    // (coef1 / (MAX_COEFFICIENT + 1)) specifies where on line 1 the intersection lies. 0 if it is at the end, 1 if it is at the beginning.
        unsigned int coef2 : 15;    // (coef2 / (MAX_COEFFICIENT + 1)) specifies where on line 2 the intersection lies. 0 if it is at the end, 1 if it is at the beginning.
        unsigned int use_line2 : 1; // true if the specified distance can be achieved by walking on line 2
        unsigned int go_forward : 1; // true if the specified distance can be achieved by walking forward (NOT USED CURRENTLY)
        unsigned int dirty : 1;     // 1 if the node received a new distance value or needs a new value.
        unsigned int distance : 15; // Specifies the distance of this intersection to home. A value of MAX_DISTANCE means infinity.
        delta_item_t delta;         // The intersection points on line1 and line2 are usually not precisely equal. This field specifies the difference between them.

        // Note that an intersection is often not exact but consists of two distinct points.
        // In this case, the distance of either point to home is upper bounded by the node distance.
        // In some cases, both points are a bit closer than this upper bound.
    };

    struct fpos_t {
        float x, y, z;
    
        inline fpos_t operator*(const float scalar) const {
            return {
                .x = this->x * scalar,
                .y = this->y * scalar,
                .z = this->z * scalar
            };
        }

        inline fpos_t operator-(const fpos_t &pos2) const {
            return {
                .x = this->x - pos2.x,
                .y = this->y - pos2.y,
                .z = this->z - pos2.z
            };
        }
    };
    
    struct ipos_t {
        int x, y, z;
        
        inline bool operator==(const ipos_t &pos2) const {
            return this->x == pos2.x && this->y == pos2.y && this->z == pos2.z;
        }

        inline ipos_t operator+(const ipos_t &pos2) const {
            return {
                .x = this->x + pos2.x,
                .y = this->y + pos2.y,
                .z = this->z + pos2.z
            };
        }

        inline ipos_t operator-(const ipos_t &pos2) const {
            return {
                .x = this->x - pos2.x,
                .y = this->y - pos2.y,
                .z = this->z - pos2.z
            };
        }

        inline ipos_t operator+=(const ipos_t &pos2) {
            this->x += pos2.x;
            this->y += pos2.y;
            this->z += pos2.z;
            return *this;
        }

        inline ipos_t operator-=(const ipos_t &pos2) {
            this->x -= pos2.x;
            this->y -= pos2.y;
            this->z -= pos2.z;
            return *this;
        }

        inline ipos_t operator-() const {
            return {
                .x = -this->x,
                .y = -this->y,
                .z = -this->z
            };
        }
    };
    

    /*** utility functions ***/

    static inline int round(float f) { return (int)(f + (f < 0 ? -0.5f : 0.5f)); };
    static inline fpos_t to_fpos(ipos_t pos) { return { .x = (float)pos.x, .y = (float)pos.y, .z = (float)pos.z }; }
    static inline ipos_t to_ipos(fpos_t pos) { return { .x = round(pos.x), .y = round(pos.y), .z = round(pos.z) }; }

    static inline bool is_close(fpos_t pos1, fpos_t pos2);
    static inline bool is_close(ipos_t pos1, fpos_t pos2);

    // Calculates the scalar product of two vectors. If any of the input vectors is very long (> ~16'000), this returns INT_MAX to prevent overflow.
    static int dot(ipos_t vec1, ipos_t vec2);

    // Calculates the squared length of a vector (the scalar product of the vector with itself).
    static inline int dot(ipos_t vec) { return dot(vec, vec); }
    
    // Determines the shortest from a point to a lines, as well as the point on the line that correspond to this distance.
    static ipos_t get_point_to_line_delta(ipos_t point, ipos_t line_delta, ipos_t line_end, int &coef);

    // Determines the shortest distance between two lines, as well as the points on the lines that correspond to this distance.
    // Either of the two coefficients can be pinned, in which case a point-to-line or point-to-point delta is returned.
    static ipos_t get_line_to_line_delta(ipos_t delta1, ipos_t end1, ipos_t delta2, ipos_t end2, int &coef1, int &coef2, bool pin_coef1, bool pin_coef2);

    // Converts a floating point number (on a scale of 0 to 1) to a discrete coefficient representation (on a scale of 0 to MAX_COEFFICIENT + 1)
    static inline int float_to_coef(float coef) { return coef * (MAX_COEFFICIENT + 1); }

    // Converts a discrete coefficient representation (on a scale of 0 to MAX_COEFFICIENT + 1) to a floating point number (on a scale of 0 to 1)
    static inline float coef_to_float(int coef) { return coef / (float)(MAX_COEFFICIENT + 1); }

    // Applies a coefficient to a delta and discretizes the result
    static ipos_t apply_coef(ipos_t delta, int coef) { return to_ipos(to_fpos(delta) * coef_to_float(coef)); }
    
    static inline delta_item_t pack_compact_delta_item(ipos_t delta);
    static inline ipos_t unpack_compact_delta_item(delta_item_t delta);

    // Returns true if the vector would be too large for a far delta element
    static inline bool fits_into_far_delta(ipos_t vec);


    /*** private functions ***/

    // Returns the desired SQUARED recording accuracy at the specified position.
    // It is conceivable that the accuracy is lower at a position further from home or at higher memory pressure.
    int get_accuracy_at(ipos_t pos);

    // Informs the tracker about a new home position.
    void set_home(float x, float y, float z);

    // Informs the tracker about a new current vehicle position.
    void update(float x, float y, float z);
    
    // Pushes a new current position to the recent path. This works even while the recent path is disabled.
    void push_recent_path(fpos_t &position);
    
    // Pops the last position from the recent path. Returns false if the path is empty.
    bool pop_recent_path(fpos_t &position);

    // Pushes a new current position to the flight graph.
    void push_graph(fpos_t &position);

    // Pushes the specified delta item to the graph. If possible, the delta is stored as a compact delta item.
    // Else, it is split into multiple items or stored as a far delta element, whichever is smaller.
    // The caller must ensure that there are at least max_space free slots at the specified index.
    // index:
    //  in: the delta will be stored starting at this index
    //  out: set to one index after the delta that was stored
    // max_space: The maximum number of slots that may be used.
    // Returns: true if the delta was pushed, false if the space was insufficient. If max_space >= FAR_DELTA_SIZE, the function always succeeds.
    bool push_delta(size_t &index, ipos_t delta, bool is_jump, size_t max_space = FAR_DELTA_SIZE);

    // Fetches a delta item from the specified index.
    // If the delta spans multiple indices, it is identified by its last index.
    ipos_t fetch_delta(size_t index, bool &is_jump);

    // Reads one delta element from the graph in the forward direction.
    // The index must point to the end of a delta element and is pre-incremented.
    ipos_t walk_forward(size_t &index, bool &is_jump);

    // Reads one delta element from the graph in the backward direction.
    // The index must point to the end of a delta element and is post-decremented.
    ipos_t walk_backward(size_t &index, bool &is_jump);
    
    // Pushes a node to the node buffer if there is enough space.
    // Returns true if the operation succeeds.
    // Returns false if there is not enough space of if there is already a nearby node. 
    bool push_node(node_t &node, int accuracy_squared);
    
    // Removes all nodes (except the home node) that reference lines within the specified range.
    // The lower bound is exclusive, the upper bound is inclusive.
    void remove_nodes(size_t lower_bound, size_t upper_bound);

    // Checks if two positions are similar.
    // This means there's at most one vertex between them, there's no jump between them and their distance along the line is not too large.
    // Returns true if these conditions are satisfied.
    bool check_similarity(size_t index1, int coef1, size_t index2, int coef2, float accuracy);

    // Returns true if the specified position is close to any line in the graph.
    //  lower_bound: exclusive lower search bound
    //  upper_bound: inclusive upper search bound
    bool is_close_to_graph(ipos_t position, size_t lower_bound, size_t upper_bound, ipos_t pos_at_bound);

    // Searches the entire graph for the position which is closest to the specified one.
    //  best_index, best_coef: specifies the index and coefficient of the closest position on the graph. Both are 0 if the graph is empty. 
    ipos_t get_closest_position(ipos_t position, size_t &best_index, unsigned int &best_coef);

    // Determines if all points between start and end lie on a straight line.
    // The points are allowed to have the distance max_deviation to the line.
    bool is_line(ipos_t start_pos, size_t start_index, ipos_t end_pos, size_t end_index, bool should_be_jump);

    // Returns the latest index to which a straight line can be drawn from start_index
    //  start_pos: the position corresponding to start_index
    //  start_index: the index at which the line should start
    //  end_pos:
    //      in: ignored
    //      out: the position corresponding to end_index
    //  end_index:
    //      in: ignored
    //      out: the index at which the longest possible line ends (start_index < end_index <= bound)
    //  is_jump: set to the jump property of the line under consideration (jump and non-jump deltas are never aggeregated)
    //  bound: the line is constrained not to go beyond this index, but it may end there.
    void get_longest_line(ipos_t start_pos, size_t start_index, ipos_t &end_pos, size_t &end_index, bool &is_jump, size_t bound);

    // Optimizes memory usage of the most recent positions in the graph.
    void consolidate_graph();

    // Makes sure that the meaning of the specified index does not change unless the graph version is incremented.
    inline size_t pin_index(size_t index) { pinned_index = index > pinned_index ? index : pinned_index; return index; }

    // Walks the graph from the specified position up to the next node, while tracking the covered distance.
    // If the search bound or a jump is encountered before a node, the function returns false.
    // Note that jumps can have nodes as well, but only at the end. Therefore, walking backwards may end at a jump node and walking forward may start at one.
    //  index, coef:
    //      in: The index/coefficient at which to start the search.
    //      out: The index/coefficient at which a node was encountered (invalid if the function returns false).
    //  distance: The distance that was covered from the start up to the node (invalid if the function returns false).
    //  forward: true if the function should walk forward, false if it should walk backward.
    //  search_bound: bounds the search (inclusive in both directions)
    bool walk_to_node(size_t &index, int &coef, float &distance, bool forward, size_t search_bound);
    
    // Uses the best node at the specified position to switch to the line which represents the shortest path home.
    // The distance to home is also returned.
    //  index, coef:
    //      in: the position at which the nodes should be examined
    //      out: the position that leads home
    //  delta: if not NULL, set to the position difference between the input and output positions
    //  go_forward: if not NULL, set to the best direction to home from the new line (valid iif the position was at a node, i.e. if the result is finite)
    float apply_node_delta(size_t &index, unsigned int &coef, ipos_t *delta, bool &go_forward);
    
    // Returns the distance to home of any position on the graph which coincides with a node,
    // including an indication on how this distance can be achieved.
    //  index, coef: the position for which the distance-to-home should be retrieved
    //  direction: 0: switch line, 1: go forward from here, -1: go backward from here
    float get_node_distance(size_t index, unsigned int coef, int &direction);
    
    // Sets the distance to home of any position on the graph which corresponds to one or multiple nodes.
    // The function takes only improving action (i.e. only reduces distances) and returns true if did so.
    //  go_forward: indicates if the distance can be achieved by walking forward
    bool set_node_distance(size_t index, int coef, float distance, bool go_forward);

    // Ensures that each node has a valid distance-to-home value.
    void refresh_distances(void);

    // Calculates the distance to home from a specific index in both forward and backward directions.
    //void get_distance_to_home(size_t index, float bias, float &dist_forward, float &dist_backward);

    // Returns the index (and bias) which is closest to the specified position.
    //size_t get_closest_index(ipos_t position, float *bias);

    // Calculates the next best move to get back home, using the provided path finding context.
    bool calc_return_path(path_finding_context_t &context, bool &progress);

    
    /*** internal variables ***/

    ipos_t home_position;
    ipos_t home_on_graph;
    bool did_set_home = false;


    bool recent_path_tracking_enabled = true;
    bool graph_tracking_enabled = true;

    
    // The most recent position in the recent path. The absolute positions in the path can be calculated based on this. 
    // If tracking has not yet started (that is, if the path is empty), this is invalid.
    fpos_t recent_path_head = { .x = 0, .y = 0, .z = 0 };
    
    // Stores the (potentially shortened) recent flight path as a ring buffer.
    // The recent path respects the following invariant: No two points are closer than ACCURACY.
    // This buffer contains only delta items. Each item stores a position relative to the previous position in the path.
    // The very first item is a bumper that indicates that the initial position (0,0,0) is not valid. This bumper will disappear once the ring buffer overflows.
    delta_item_t recent_path[RECENT_PATH_LENGTH] = { 0xFFFF };
    
    size_t recent_path_next_write = 1; // always valid, 0 if empty, equal to next_read if full
    size_t recent_path_next_read = 0; // RECENT_PATH_LENGTH if empty, valid if non-empty


    // The last position in the graph, corresponding to the end of the buffer.
    // This roughly corresponds to the current vehicle position.
    // The absolute positions in the path can be calculated based on this.
    ipos_t graph_head_pos = { .x = 0, .y = 0, .z = 0 };

    // The next free index in the graph.
    size_t graph_next_write = 0;

    // Stores the entire flight path (until the buffer is full).
    // Each delta item represents a line from the previous position to a new position.
    // Sometimes (if the vehicle moves along an already visited path), jump deltas are inserted.
    // Note that the first item carries no valid information other than that the graph is non-empty.
    delta_item_t graph[GRAPH_LENGTH];

    // The consolidated head position represents the last position which shall no longer be considered by the consolidation pass.
    // Moving this around arbitrarily should not result in undefined behaviour. 
    ipos_t consolidated_head_pos = { .x = 0, .y = 0, .z = 0 };
    size_t consolidated_head_index = 0;

    // If the graph is altered in a way that previously exported indices become invalid, the graph version is incremented.
    int graph_version = 0;

    // If this index or anything before changes, the graph version must be incremented.
    size_t pinned_index = 0;


    // Nodes keep track of lines in the flight path that pass close to each other.
    // The first node (index1, coef1) represents the home position and must not be removed.
    node_t nodes[NODE_BUFFER_SIZE] = {
        {
            .index1 = 0,
            .index2 = 0,
            .coef1 = 0,
            .coef2 = 0,
            .use_line2 = 0,
            .dirty = 1,
            .distance = 0 
        }
    };
    size_t node_count = 1;

    // True as long as any nodes in the buffer have dirty set to 1
    bool have_dirty_nodes = true;

    // The end of the controlled area. All nodes beyond this area shall be considered dirty.
    // This is always larger than the greatest dirty node.
    size_t consolidated_area_end = 0;
    
    // The distance-to-home at the end of the controlled area
    float controlled_area_end_distance = 0;
    
};

struct Tracker::path_finding_context_t {
    ipos_t current_pos; // The position corresponding to (current_index, current_coef)
    size_t current_index;
    unsigned int current_coef;
    size_t checkpoint_index; // If (current_index, current_coef) becomes equal to this checkpoint, a re-evaluation of the best direction is forced, which may involve switching lines on an intersection. 
    unsigned int checkpoint_coef;

    int graph_version;
};

#endif // NAVIGATOR_TRACKER_H
