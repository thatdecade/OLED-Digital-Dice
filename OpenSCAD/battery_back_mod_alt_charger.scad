/* Digital Dice - Facelift
 
   Author: Dustin Westaby
   
   This is the back side of the dice, modified to hold a USB charger circuit.
   * https://amzn.to/3uVuUw5 - USB C Charger
   * https://amzn.to/34X2uac - 5V step Up Circuit
   
   Warning: By using a step up circuit not integrated into the charger, you must enable the low battery protection in software.

*/

cover_programming_usb = 1; //0 for uncovered, 1 for covered

sliding_fit = 2;
friction_fit = 0.2;

//dimensions
outer_shell_z = 25;
inner_shell_x = 40;

back_wall_thickness = 3;
side_wall_thickness = 3;

battery_shield_distance_from_side_wall = 7;

switch_hole_diameter = 12;

usb_opening_width  = 9 + sliding_fit;
usb_opening_height = 3.26 + friction_fit;

usb_x_offset_from_center = inner_shell_x/2;
usb_y_offset_for_pcb_thickness = 1;

battery_x = 5 + sliding_fit;
battery_y = 30 + friction_fit;
battery_z = 35 + sliding_fit;

battery_lid_wall_thickness = 1.2;
    
battery_y_offset = -0.6; //reduce overlap with die button clearance

//used to ensure wiring clearance
DIE_switch_x = 12 - 22;
DIE_switch_y = 30 + side_wall_thickness - friction_fit;
DIE_switch_z = 12;

usb_board_width = 17.34 + friction_fit;
usb_socket_width = 2;
pcb_thickness = 0.9;
usb_pcb_height = 4.3;
pcb_length = 29.53 + friction_fit - 1;

difference()
{
    union()
    {
        translate([0,25,49.6]) rotate([-90,0,0]) import("back.stl");
        //translate([0,25,49.6]) rotate([-90,0,0]) import("front.stl");
        draw_battery_lid();
        if (cover_programming_usb) draw_usb_cover();
            
        draw_charger_push_stop();
    }
    
    draw_power_switch();
    
    draw_charger_usb_slot();

}

module draw_charger_push_stop()
{
    stop_z = usb_board_width + side_wall_thickness +4;
    
    translate([usb_x_offset_from_center,pcb_length+pcb_thickness-0.1,0]) 
    cube([back_wall_thickness,back_wall_thickness,stop_z]);
}

module draw_charger_usb_slot()
{
    
    usb_length_from_pcb_front = 0.9;
    
    translate([
    usb_x_offset_from_center - pcb_thickness - usb_opening_height/2,
    -0.1,0]) 
    {
    
        //usb slot
        translate([pcb_thickness,0,usb_board_width/2] )cube([usb_opening_height,4,usb_opening_width], center=false);
        
        //usb charger pcb
        translate([usb_pcb_height/2,usb_length_from_pcb_front,usb_opening_width/2]) cube([3,pcb_length,usb_board_width], center=false); //TBD
    }
        
}

module draw_usb_cover()
{
usb_opening_width  = 8.5;
back_wall_thickness = 1.4;
usb_opening_height = 3;
    
    translate([-usb_opening_width/2-0.5,back_wall_thickness+0.5,0]) cube([usb_opening_width+1,usb_opening_height+1,0.2]);
}

module draw_battery_lid()
{
    //battery sleave / cover
    battery_x_wall = battery_x + battery_lid_wall_thickness;
    battery_y_wall = battery_y + battery_lid_wall_thickness;
    battery_z_wall = battery_z;

    translate([-22,3,3]) 
    difference() 
    {
        //shell
        cube([battery_x_wall,battery_y_wall+battery_y_offset,battery_z_wall]);
        
        //battery
        translate([-0.1,battery_y_offset-0.1,0.1]) cube([battery_x+0.1,battery_y+0.1,battery_z+0.1]);
        
        //chamfer sides to fit other shell
        translate([0,-0.1,outer_shell_z]) cube([battery_x_wall+0.1,friction_fit+0.1,battery_z_wall - outer_shell_z + 0.1]);
        translate([-0.1,0,outer_shell_z]) cube([friction_fit+0.1,battery_y_wall+0.1,battery_z_wall - outer_shell_z + 0.1]);
        
    }
}

module draw_power_switch()
{
    //power switch hole
    translate([12,25/2*3,-0.1]) cylinder(h=4, d=switch_hole_diameter, $fn=100);
    
    //DIE switch clearance
    translate([0,0,3]) translate([DIE_switch_x,DIE_switch_y,DIE_switch_z]) rotate([-90,0,0]) cylinder(h=25, d=switch_hole_diameter, $fn=50);
    
    //battery clearance 
    translate([-22,3,3]) translate([-0.1,battery_y_offset-0.1,0.1]) cube([battery_x+0.1,battery_y+0.1,battery_z+0.1]);
}
