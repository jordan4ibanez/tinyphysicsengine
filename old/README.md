WIP

NOTE to self and other: This probably isn't gonna work, 32bit fixed point has
showed to be too low precision for a 3D physics engine (even though it is enough
for 3D rendering). The issue is that we need to represent both very high and
very low energies so we can allocate about 9 bits to the fractional part of the
number, however this is too little to deal with slow rotations and low energies;
it happens too often that for example objects balance on their edges because
their energy gets quite small and rounded to zero. The project probably needs to
be rewritten from scratch with more precision (still wouldn't use floating point
but rather some tiny custon fixed point library).
