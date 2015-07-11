geometry = d8_stokes.geo
# and mesh
mesh = d8_stokes.vol.gz
# geometry = d99_testgeom.in2d
# mesh = d99_testgeom_unstr.vol.gz
# mesh = d99_testgeom_unstr.vol.gz
shared = libngsxfem_xfem

define constant heapsize = 1e9
constant R = 0.5-1e-12

define coefficient lset
( sqrt((x)*(x)+y*y+z*z) - R),


######### CURV IT #########
        
fespace fes_p1 -type=h1ho -order=1
gridfunction lset_p1 -fespace=fes_p1

fespace fes_ho -type=h1ho -order=2
gridfunction lset_ho -fespace=fes_ho
                
numproc setvalues npsv -gridfunction=lset_ho -coefficient=lset

fespace fes_deform -type=h1ho -order=2 -vec -dirichlet=[1,2,3,4,5,6]
gridfunction deform -fespace=fes_deform
                
numproc xgeomtest3d npxd 
        -gf_levelset_p1=lset_p1
        -gf_levelset_ho=lset_ho
        # -levelset=lset
        -gf_levelset=lset_ho
        -deformation=deform
        # -dynamic_search_dir
        # -nocutoff
        -threshold=0.05
        -reject_threshold=1
        -volume=0.52359878
        -lower_lset_bound=0.0
        -upper_lset_bound=0.0
######### CURV IT #########
        

define constant aneg = 2.0
define constant apos = 1.0


define coefficient rhsneg
(12),

define coefficient rhspos
(4/(sqrt(x*x+y*y+z*z))),

define coefficient solpos
(1.0-2.0*sqrt(x*x+y*y+z*z)),

define coefficient solneg
(1.0/4.0-(x*x+y*y+z*z)),


# define fespace fescomp
#        -type=xstdfespace
#        -type_std=h1ho 
#        -order=2
#        -dirichlet=[1,2,3,4,5,6]
#        -ref_space=0
# #       -dgjumps

# numproc informxfem npix 
#         -xstdfespace=fescomp
#         -coef_levelset=lset

# define gridfunction u -fespace=fescomp

# numproc setvaluesx npsvx -gridfunction=u -coefficient_neg=solneg -coefficient_pos=solpos -boundary #-print
        

# define linearform f -fespace=fescomp # -print
# xsource rhsneg rhspos
# # xsource solneg solpos

# define bilinearform a -fespace=fescomp -printelmat #-eliminate_internal -keep_internal -symmetric -linea
# # xmass 1.0 1.0
# xlaplace aneg apos
# xnitsche_heaviside aneg apos 1.0 1.0 10.0
        
# define preconditioner c -type=direct -bilinearform=a

# numproc bvp npbvp -gridfunction=u -bilinearform=a -linearform=f -solver=cg -preconditioner=c -maxsteps=1000 -prec=1e-6 # -print

                
numproc draw npdr -coefficient=lset -label=levelset


numproc visualization npvis -scalarfunction=lset_p1 -vectorfunction=deform -deformationscale=1 -subdivision=0 -minval=0 -maxval=0

coefficient zero
0,

# numproc xdifference npxd 
#         -solution=u 
#         -solution_n=solneg
#         -solution_p=solpos
#         -jumprhs=zero
#         -levelset=lset
#         -interorder=5
#         -henryweight_n=1
#         -henryweight_p=1
#         -diffusion_n=2
#         -diffusion_p=1

# coefficient err
# (
#   ((lset) > 0) * (abs((u)-solpos)) 
#  +((lset) < 0) * (abs((u)-solneg)) 
# )

# numproc draw npdraw -coefficient=err -label=error
        
numproc unsetdeformation npunset

coefficient deformx
((deform)*(1,0,0)),

coefficient deformy
((deform)*(0,1,0)),

coefficient deformz
((deform)*(0,0,1)),
                        
# numproc vtkoutput npout -filename=nosub_d101
#         -coefficients=[lset,deformx,deformy,deformz]
#         -gridfunctions=[lset_p1,lset_ho]
#         -fieldnames=[levelset,deform_x,deform_y,deform_z,levelset_p1,levelset_ho]
#         -subdivision=0
        
        