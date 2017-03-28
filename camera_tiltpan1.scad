// base
cube([76,76,3]);

// pan servo
translate([19,28,3])
servo();

// tilt servo
translate([19,75,70])
rotate([90,90,0])
servo();


module servo()
{
union() {
	cube([38,19,25]);

	// shaft
	translate([8,9,25])
	cylinder(h=2,r=1);

	// shaft platform
	translate([8,9,27])
	cylinder(h=2,r=18);
}
}