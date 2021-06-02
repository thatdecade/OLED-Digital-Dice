/* Digital Dice - Facelift
 
   Author: Dustin Westaby

*/

cover_programming_usb = 0; //0 for uncovered, 1 for covered

sliding_fit = 2;
friction_fit = 0.2;

//dimensions
outer_shell_z = 25;

back_wall_thickness = 1.4; //1.3 //wemos usb is 2 layer heights, charger needs 6 layer heights
side_wall_thickness = 3;

battery_shield_distance_from_side_wall = 7;

switch_hole_diameter = 12;

usb_opening_width  = 8.5;
usb_opening_height = 3;
usb_x_offset_from_center = 5.75;
usb_y_offset_for_pcb_thickness = 1;

usb_board_width  = 23.66+2;
usb_board_height = 6.82+2;

battery_x = 5 + sliding_fit;
battery_y = 30 + friction_fit;
battery_z = 35 + sliding_fit;

battery_lid_wall_thickness = 1.2;
    
battery_y_offset = -0.6; //reduce overlap with die button clearance

//used to ensure wiring clearanceW
DIE_switch_x = 12;
DIE_switch_y = 30;
DIE_switch_z = 12;

difference()
{
    union()
    {
        translate([0,25,49.6]) rotate([-90,0,0]) import("back.stl");
        draw_battery_lid();
        if (cover_programming_usb) draw_usb_cover();
    }
    
    draw_charger_usb_slot();
    draw_charger_shield();
    draw_power_switch();

}

module draw_usb_cover()
{
    translate([-usb_opening_width/2-0.5,back_wall_thickness+0.5,0]) cube([usb_opening_width+1,usb_opening_height+1,0.2]);
}

module draw_power_switch()
{
    //power switch hole
    translate([12,25/2*3,-0.1]) cylinder(h=4, d=switch_hole_diameter, $fn=100);
    
    //DIE switch clearance
    translate([-22,3,3]) translate([DIE_switch_x,DIE_switch_y,DIE_switch_z]) rotate([-90,0,0]) cylinder(h=80, d=switch_hole_diameter, $fn=50);
    
    //battery clearance 
    translate([-22,3,3]) translate([-0.1,battery_y_offset-0.1,0.1]) cube([battery_x+0.1,battery_y+0.1,battery_z+0.1]);
}

module draw_charger_shield()
{
    inner_tab_thickness = 0.6;
    
    //battery board
    translate([-usb_board_width/2,side_wall_thickness+battery_shield_distance_from_side_wall-inner_tab_thickness,back_wall_thickness]) 
    cube([usb_board_width,usb_board_height+inner_tab_thickness,40], center=false);
    
    //inner tab
}

module draw_charger_usb_slot()
{
    //usb slot
    translate([-usb_opening_width/2-usb_x_offset_from_center,side_wall_thickness+battery_shield_distance_from_side_wall+usb_y_offset_for_pcb_thickness,-0.1]) 
    cube([usb_opening_width,usb_opening_height,4], center=false);
        
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
        
        //switch clearance
        translate([DIE_switch_x,DIE_switch_y,DIE_switch_z]) rotate([-90,0,0]) cylinder(h=80, d=switch_hole_diameter, $fn=50);
        
        //chamfer sides to fit other shell
        translate([0,-0.1,outer_shell_z]) cube([battery_x_wall+0.1,friction_fit+0.1,battery_z_wall - outer_shell_z + 0.1]);
        translate([-0.1,0,outer_shell_z]) cube([friction_fit+0.1,battery_y_wall+0.1,battery_z_wall - outer_shell_z + 0.1]);
        
    }
}
