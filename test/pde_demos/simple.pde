#
# solve the Poisson equation -Delta u = f
#
# with boundary conditions
#      u = 0  on Gamma1
#  du/dn = 1  on Gamma2

# load geometry
geometry = square.in2d

# and mesh
#mesh = square.vol.gz
#mesh = square_trigs.vol.gz
mesh = square_quad_coarse.vol.gz

shared = libngsxfem_test
shared = libngsxfem_common

define constant one = 1.0
define constant zero = 0.0

define constant cuteps = 0.0

define variable t = 0.0

define constant told = 0.0
define constant tnew = 0.1

define coefficient lset
(cos(2*pi*(x))),

define fespace fes_st 
       -type=spacetimefes 
       -type_space=l2ho
       -order_space=5
       -all_dofs_together
       -order_time=5
#       -print
#       -dirichlet=[2]

define gridfunction u_st -fespace=fes_st

define bilinearform a -fespace=fes_st -printelmat -print
STtimeder one
#STlaplace told tnew one
#STmass told tnew one
STtracepast one
#STtracefuture one
#STtracemass one one
#STtracemass zero one

define linearform f -fespace=fes_st -printelvec #-print
#STsource told tnew lset
STtracesource zero one
#STtracesource one lset

numproc bvp nps -bilinearform=a -linearform=f -gridfunction=u_st -solver=direct -print

# numproc testxfem nptxfem 
#     -levelset=lset 
#     -fespace=fes_st
#     -approx_order_space=1
#     -spacetime 
#     -time=t
#     -timeinterval=[0,1]
#     -approx_order_time=0
#     -order_time=1


define bilinearform evalu_past -fespace=fes_st 
STtracepast zero

define bilinearform evalu_future -fespace=fes_st 
STtracefuture zero

numproc drawflux npdf -bilinearform=evalu_past -solution=u_st -label=u_past
numproc drawflux npdf -bilinearform=evalu_future -solution=u_st -label=u_future

numproc visualization npvis -scalarfunction=u_future -nolineartexture #-minval=0.0 -maxval=1.0
