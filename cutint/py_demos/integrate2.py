# integration on lset domains

from math import pi
# ngsolve stuff
from ngsolve import *
# basic xfem functionality
from xfem.basics import *


NAveraging = 20

def PrintTimers(substring):
    ### print hdg-intergrator timers
    hdgtimers = [a for a in Timers() if substring in a["name"]]
    #hdgtimers = sorted(hdgtimers, key=lambda k: k["name"], reverse=False)
    hdgtimers = sorted(hdgtimers, key=lambda k: k["time"], reverse=True)
    for timer in hdgtimers:
        print("{:<45}: {:6} cts, {:.8f} s, {:.6e} s(avg.)"
              .format(timer["name"],timer["counts"],timer["time"],timer["time"]/(max(1.0,timer["counts"]))))

#cube domain [-1,1]x[-1,1]x[-1,1]
def Make3DProblem(maxh):
    from netgen.csg import CSGeometry, OrthoBrick, Pnt
    cube = OrthoBrick( Pnt(-1,-1,-1), Pnt(1,1,1) ).bc(1)
    geom = CSGeometry()
    geom.Add (cube)
    mesh = Mesh (geom.GenerateMesh(maxh=maxh, quad_dominated=False))
    return mesh

# sphere with radius 0.5
levelset = (sqrt(x*x+y*y+z*z)-0.5) #.Compile()
#levelset = x

referencevals = { POS : 8.0-pi/6.0, NEG : pi/6.0, IF : pi }

mesh = Make3DProblem(maxh=0.22)

V = H1(mesh,order=1)
lset_approx = GridFunction(V)
InterpolateToP1(levelset,lset_approx)

domains = [NEG,POS,IF]
#domains = [IF]

errors = dict()
eoc = dict()

for key in domains:
    errors[key] = []
    eoc[key] = []

#now: repetitions to average performance (later convergence..)
for reflevel in range(NAveraging):

    # if(reflevel > 0):
    #     mesh.Refine()

    f = CoefficientFunction (1.0)

    for key in domains:
        integral_old = Integrate(levelset_domain={"levelset" : levelset, "domain_type" : key}, cf=f, mesh=mesh, order=0)
        #integral_old = IntegrateX(lset=levelset,mesh=mesh,cf_neg=f, cf_pos=f, cf_interface=f,order=0,subdivlvl = 0,domains=interface_and_volume_domains)["interface"]
        #integral_old = NewIntegrateX(lset=lset_approx,mesh=mesh,cf=f,order=0,domain_type=key,heapsize=1000000, int_old=True)
        #print("\n\n ----- NOW STARTING WITH THE NEWINTEGRATEX-FUNCTION -----\n\n")
        integral = NewIntegrateX(lset=lset_approx,mesh=mesh,cf=f,order=0,domain_type=key,heapsize=1000000, int_old=False)

        #integral = integral_old
        #integral_old = integral

        if abs(integral_old - integral) > 1e-14:
            print("accuracy not sufficient...")
            print("Results: ", integral_old, integral)
            input("press enter to continue...")

        errors[key].append(abs(integral - referencevals[key]))

for key in domains:
    eoc[key] = [log(a/b)/log(2) for (a,b) in zip (errors[key][0:-1],errors[key][1:]) ]

print("errors:  \n{}\n".format(  errors))
print("   eoc:  \n{}\n".format(     eoc))

Draw(levelset,mesh,"levelset")

PrintTimers("IntegrateX")

PrintTimers("StraightCutIntegrationRule")
#PrintTimers("StraightCutDomain")
#PrintTimers("MakeQuadRuleFast")
#PrintTimers("Simplex::CheckifCut")
#PrintTimers("PointContainer")
#PrintTimers("CutSimplex<2>::MakeQuad")
#PrintTimers("MakeQuadRuleOnCutSimplex")
#PrintTimers("StraightCutElementGeometry")
#PrintTimers("CheckIfCutFast")
