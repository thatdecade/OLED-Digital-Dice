/* Digital Dice - Facelift
 
   Author: Dustin Westaby

*/

back_wall_thickness = 1.3;
side_wall_thickness = 2.4;

battery_shielf_distance_from_wemos_pcb = 11.06;

usb_opening_width  = 8.5;
usb_opening_height = 3;
usb_x_offset_from_center = 7;

usb_board_width  = 23.66+2;
usb_board_height = 6.82+2;

difference()
{
translate([0,25,49.6]) rotate([-90,0,0]) import("back.stl");

//usb slot
translate([-usb_opening_width/2-usb_x_offset_from_center,side_wall_thickness+battery_shielf_distance_from_wemos_pcb,-0.1]) cube([usb_opening_width,usb_opening_height,4], center=false);
    
    //battery board
translate([-usb_board_width/2,side_wall_thickness+battery_shielf_distance_from_wemos_pcb,back_wall_thickness]) cube([usb_board_width,usb_board_height,4], center=false);
    
translate([12,25/2*3,-0.1]) cylinder(h=4, d=12, $fn=100);
}