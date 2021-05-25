/* Digital Dice - Facelift
 
   Author: Dustin Westaby

   This script allows the dice face to be parameterized to fit any display size. 

*/

wall_thickness = 0.1;

old_display_size_x = 28;
old_display_size_y = 5;
old_display_size_z = 28;

//import("flexbatter1xAAAx3.stl");

translate([0,0,0.45]) rotate([90,0,0])
difference()
{
import("front.stl");

translate([0,old_display_size_y/2+wall_thickness,-4]) cube([old_display_size_x,old_display_size_y,old_display_size_z], center=true);
}