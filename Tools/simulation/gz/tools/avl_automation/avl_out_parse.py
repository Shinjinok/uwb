#!/usr/bin/env
# The idea of this file is to read the generated output file
# parameters and use them to generate the necessary plugin parameters for the advanced_lift_drag plugin.

import argparse
import shutil
import fileinput
import subprocess
import os

def get_coef(file,token):

    linesplit = []
    for line in file:
        if f' {token} ' in line:
            linesplit = line.split()
            break

    index = 0
    for i,v in enumerate(linesplit):
        if v == token:
            index = i
    value = linesplit[index+2]
    return value


# This function is used to write the gathered coefficient values
# to the file containing the plugin.
def write_coef(file, token_str, token):
    old_line = f'<{token_str}></{token_str}>'
    new_line = f'<{token_str}>{str(token)}</{token_str}>'
    with fileinput.FileInput(file, inplace=True) as output_file:
        for line in output_file:
            print(line.replace(old_line, new_line), end='')



def ctrl_surface_coef(file,ctrl_surface_vec,index,direction):

    extracted_text = ''
    with open("./templates/control_surface.sdf",'r') as open_file:
        for line in open_file:
            extracted_text += line
        open_file.close()

    extracted_text = extracted_text.replace("<name></name>",f'<name>servo_{index}</name>')
    extracted_text = extracted_text.replace("<index></index>",f'<index>{index}</index>')
    extracted_text = extracted_text.replace("<direction></direction>",f'<directon>{direction}</direction>')
    extracted_text = extracted_text.replace("<CD_ctrl></CD_ctrl>",f'<CD_ctrl>{ctrl_surface_vec[0]}</CD_ctrl>')
    extracted_text = extracted_text.replace("<CY_ctrl></CY_ctrl>",f'<CY_ctrl>{ctrl_surface_vec[1]}</CY_ctrl>')
    extracted_text = extracted_text.replace("<CL_ctrl></CL_ctrl>",f'<CL_ctrl>{ctrl_surface_vec[2]}</CL_ctrl>')
    extracted_text = extracted_text.replace("<Cell_ctrl></Cell_ctrl>",f'<Cell_ctrl>{ctrl_surface_vec[3]}</Cell_ctrl>')
    extracted_text = extracted_text.replace("<Cem_ctrl></Cem_ctrl>",f'<Cem_ctrl>{ctrl_surface_vec[4]}</Cem_ctrl>')
    extracted_text = extracted_text.replace("<Cen_ctrl></Cen_ctrl>",f'<Cen_ctrl>{ctrl_surface_vec[5]}</Cen_ctrl>')


    # Create model specific template
    with open(file,'a') as plugin_file:
        plugin_file.write(extracted_text + "\n")
        plugin_file.close()



def main(file_name,vehicle_type,AR,mac,ref_pt_x,ref_pt_y,ref_pt_z,num_ctrl_surfaces,area,ctrl_surface_order):
    #parameters present when using ST in JVL:
    result = subprocess.run(['pwd'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    if result.returncode == 0:
        # Save the output in a variable
        savedir = result.stdout.strip()
        # print(f"Current working directory is: {current_path}")

    user = os.environ.get('USER')
    filedir = f'/home/{user}/Avl/runs/'

    with open(f'{filedir}custom_vehicle_stability_derivatives.txt','r+') as stability_file:
        original_position = stability_file.tell()

        # As plane is modelled at 0 degree AoA, the total coefficients should(?) correspond to the
        # 0 degree coefficients required by the plugin.
        alpha = get_coef(stability_file,"Alpha")
        Cm0 = get_coef(stability_file,"Cmtot")
        CL0 = get_coef(stability_file,"CLtot")
        CD0 = get_coef(stability_file,"CDtot")

        CLa = get_coef(stability_file,"CLa")
        CYa = get_coef(stability_file,"CYa")
        Cella = get_coef(stability_file,"Cla")
        Cema = get_coef(stability_file,"Cma")
        Cena = get_coef(stability_file,"Cna")

        stability_file.seek(original_position)

        CLb = get_coef(stability_file,"CLb")
        CYb = get_coef(stability_file,"CYb")
        Cellb = get_coef(stability_file,"Clb")
        Cemb = get_coef(stability_file,"Cmb")
        Cenb = get_coef(stability_file,"Cnb")
        stability_file.close()

    with open(f'{filedir}custom_vehicle_body_axis_derivatives.txt') as bodyax_file:
        original_position = bodyax_file.tell()

        eff = get_coef(bodyax_file,"e")

        bodyax_file.seek(original_position)

        CDp = get_coef(bodyax_file,"CXp")
        CYp = get_coef(bodyax_file,"CYp")
        CLp = get_coef(bodyax_file,"CZp")
        Cellp = get_coef(bodyax_file,"Clp")
        Cemp = get_coef(bodyax_file,"Cmp")
        Cenp = get_coef(bodyax_file,"Cnp")

        bodyax_file.seek(original_position)

        CDq = get_coef(bodyax_file,"CXq")
        CYq = get_coef(bodyax_file,"CYq")
        CLq = get_coef(bodyax_file,"CZq")
        Cellq = get_coef(bodyax_file,"Clq")
        Cemq = get_coef(bodyax_file,"Cmq")
        Cenq = get_coef(bodyax_file,"Cnq")

        bodyax_file.seek(original_position)

        CDr = get_coef(bodyax_file,"CXr")
        CYr = get_coef(bodyax_file,"CYr")
        CLr = get_coef(bodyax_file,"CZr")
        Cellr = get_coef(bodyax_file,"Clr")
        Cemr = get_coef(bodyax_file,"Cmr")
        Cenr = get_coef(bodyax_file,"Cnr")
        bodyax_file.close()

    plane_type = vehicle_type
    ctrl_surface_mat = []

    match plane_type:

        case "custom":
            ctrl_surface_vec = []
            with open(f'{filedir}custom_vehicle_body_axis_derivatives.txt') as bodyax_file:
                original_position = bodyax_file.tell()
                for i in range(1,4):
                    ctrl_surface_vec = []
                    ctrl_surface_vec.append(get_coef(bodyax_file,f'CXd{i}'))
                    ctrl_surface_vec.append(get_coef(bodyax_file,f'CYd{i}'))
                    ctrl_surface_vec.append(get_coef(bodyax_file,f'CZd{i}'))
                    ctrl_surface_vec.append(get_coef(bodyax_file,f'Cld{i}'))
                    ctrl_surface_vec.append(get_coef(bodyax_file,f'Cmd{i}'))
                    ctrl_surface_vec.append(get_coef(bodyax_file,f'Cnd{i}'))
                    bodyax_file.seek(original_position)
                    ctrl_surface_mat.append(ctrl_surface_vec)


        # case "standard_vtol":
        # case "custom"

# SPECIFY STALL PARAMETERS BASED ON AIRCRAFT TYPE (IF PROVIDED)
    if not os.path.exists(f'{savedir}/{file_name}'):
        os.makedirs(f'{savedir}/{file_name}')
    file_name = f'{savedir}/{file_name}/{file_name}.sdf'
    shutil.copy(f'{savedir}/templates/advanced_lift_drag_template.sdf',file_name)

    # Get argument coefficients taken directly from the input file.
    write_coef(file_name,"a0",alpha)
    write_coef(file_name,"CL0",CL0)
    write_coef(file_name,"CD0",CD0)
    write_coef(file_name,"Cem0",Cm0)
    write_coef(file_name,"AR",AR)
    write_coef(file_name,"area",area)
    write_coef(file_name,"mac",mac)
    write_coef(file_name,"air_density",1.2041) # TODO: Provide custom air density option
    write_coef(file_name,"forward","1 0 0")
    write_coef(file_name,"upward","0 0 1")
    write_coef(file_name,"link_name","base_link")
    write_coef(file_name,"cp",f'{ref_pt_x} {ref_pt_y} {ref_pt_z}')
    write_coef(file_name,"num_ctrl_surfaces",num_ctrl_surfaces)

    write_coef(file_name,"CLa",CLa)
    write_coef(file_name,"CYa",CYa)
    write_coef(file_name,"Cella",Cella)
    write_coef(file_name,"Cema",Cema)
    write_coef(file_name,"Cena",Cena)
    write_coef(file_name,"CLb",CLb)
    write_coef(file_name,"CYb",CYb)
    write_coef(file_name,"Cellb",Cellb)
    write_coef(file_name,"Cemb",Cemb)
    write_coef(file_name,"Cenb",Cenb)

    write_coef(file_name,"CDp",CDp)
    write_coef(file_name,"CYp",CYp)
    write_coef(file_name,"CLp",CLp)
    write_coef(file_name,"Cellp",Cellp)
    write_coef(file_name,"Cemp",Cemp)
    write_coef(file_name,"Cenp",Cenp)
    write_coef(file_name,"CDq",CDq)
    write_coef(file_name,"CYq",CYq)
    write_coef(file_name,"CLq",CLq)
    write_coef(file_name,"Cellq",Cellq)
    write_coef(file_name,"Cemq",Cemq)
    write_coef(file_name,"Cenq",Cenq)
    write_coef(file_name,"CDr",CDr)
    write_coef(file_name,"CYr",CYr)
    write_coef(file_name,"CLr",CLr)
    write_coef(file_name,"Cellr",Cellr)
    write_coef(file_name,"Cemr",Cemr)
    write_coef(file_name,"Cenr",Cenr)

    write_coef(file_name,"eff",eff)

    # TODO: Improve this for custom stall values
    # Note: Currently these stall values are simply taken from advanced_plane presets.

    write_coef(file_name,"alpha_stall","0.3391428111")
    write_coef(file_name,"CLa_stall","-3.85")
    write_coef(file_name,"CDa_stall","-0.9233984055")
    write_coef(file_name,"Cema_stall","0")

    # TODO: Get this to work for more custom frames.

    # Check whether a particular type of control surface has been seen before. If it has,
    # then the current control surface is the (right) counterpart.

    # ASSUMPTION: There is the assumption that an vehicle will only ever have two of any
    # particular type of control surface. (left and right). If this is not the case, the negation
    # below will likely not work correctly.
    type_seen = list()

    # Dictionary containing the directions that each type of control surface can move
    ctrl_direction = {"aileron": 1,"elevator": -1,"rudder": 1}

    match plane_type:

        case "custom":
            for i, ctrl_surface in enumerate(ctrl_surface_order):

                # Check whether a particular type of control surface has been seen before. If it has,
                # then the current control surface is the (right) counterpart. Depending on the exact
                # nature of the encountered type you then need to negate the correct parameters.
                if ctrl_surface in type_seen:
                    # Work out what the corresponding index for the first encounter of the ctrl surface is.
                    seen_index = type_seen.index(ctrl_surface)

                    if ctrl_surface == 'aileron':
                        #Change for right wing aileron by flipping sign
                        ctrl_surface_mat[seen_index][3] = -float(ctrl_surface_mat[0][3])
                        ctrl_surface_mat[seen_index][5] = -float(ctrl_surface_mat[0][5])

                    # Split Elevators are assumed to never run differentially. Feel free to add a
                    # condition if your plane does require differential elevator action.

                else:
                    # If a ctrl surface has not been encountered add it to the type_seen list and
                    # set the index to the length of the list - 1 as this corresponds to the newest
                    # unseen element in ctrl_surface_mat .
                    type_seen.append(ctrl_surface)
                    seen_index = len(type_seen) - 1

                ctrl_surface_coef(file_name,ctrl_surface_mat[seen_index],i,ctrl_direction[ctrl_surface])


        # case "standard_vtol":

        # case "cessna":


    # close the sdf file with plugin
    with open(file_name,'a') as plugin_file:
        plugin_file.write("</plugin>")
        plugin_file.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument("file_name")
    parser.add_argument("vehicle_type")
    parser.add_argument("AR")
    parser.add_argument("mac")
    parser.add_argument("ref_pt_x")
    parser.add_argument("ref_pt_y")
    parser.add_argument("ref_pt_z")
    parser.add_argument("num_ctrl_surfaces")
    parser.add_argument("area")
    parser.add_argument("ctrl_surface_order")

    args = parser.parse_args()

    main(args.file_name,args.vehicle_type,args.AR,args.mac,args.ref_pt_x,args.ref_pt_y,
         args.ref_pt_z,args.num_ctrl_surfaces,args.area,args.ctrl_surface_order)
