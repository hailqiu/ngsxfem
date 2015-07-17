#include "shiftintegrators.hpp"

namespace ngfem
{

  template <int D>
  ShiftIntegrator<D> :: ShiftIntegrator (const Array<shared_ptr<CoefficientFunction>> & coeffs)
    : coef_lset_p1(coeffs[0]),coef_lset_ho(coeffs[1]) 
  {
    
    if (coeffs.Size() > 2)
    {
      std::cout << " asdf " << std::endl;
      max_deform = coeffs[2]->EvaluateConst();
    }
    if (coeffs.Size() > 3)
    {
      std::cout << " asdf " << std::endl;
      lower_lset_bound = coeffs[3]->EvaluateConst();
    }
    if (coeffs.Size() > 4)
    {
      std::cout << " asdf " << std::endl;
      upper_lset_bound = coeffs[4]->EvaluateConst();
    }
    if (coeffs.Size() > 5)
    {
      std::cout << " asdf " << std::endl;
      qn = coeffs[5];
    }
  }

  template <int D>
  void ShiftIntegrator<D> :: CalcElementVector (const FiniteElement & fel,
                                                const ElementTransformation & eltrans,
                                                FlatVector<double> elvec,
                                                LocalHeap & lh) const
  {
    elvec = 0.0;
    const ScalarFiniteElement<D> & scafe = dynamic_cast<const ScalarFiniteElement<D> &>(fel);
      
    FlatMatrixFixWidth<D> elvecmat(scafe.GetNDof(),&elvec(0));
    elvecmat = 0.0;

    ELEMENT_TYPE et = eltrans.GetElementType();
    bool has_neg = false;
    bool has_pos = false;
    for (int s = 0; s < ElementTopology::GetNVertices(et); ++s)
    {
      const double * v = ElementTopology::GetVertices(et)[s];
      IntegrationPoint ip(v[0],v[1],v[2]);
      MappedIntegrationPoint<D,D> mip(ip, eltrans);
      const double val = coef_lset_p1->Evaluate(mip);
      if (val==0.0) has_neg = has_pos = true;
      if (val > lower_lset_bound)
        has_pos = true;
      if (val < upper_lset_bound)
        has_neg = true;
    }
    if (!has_neg || !has_pos)
      return;
      
    FlatVector<> shape (scafe.GetNDof(),lh);
      
    IntegrationRule ir = SelectIntegrationRule (eltrans.GetElementType(), 2*scafe.Order());
    for (int l = 0 ; l < ir.GetNIP(); l++)
    {
      MappedIntegrationPoint<D,D> mip(ir[l], eltrans);
      scafe.CalcShape(ir[l],shape);

      Vec<D> grad;
      CalcGradientOfCoeff(coef_lset_p1, mip, grad, lh);
      Mat<D> trafo_of_normals = mip.GetJacobianInverse() * Trans(mip.GetJacobianInverse());
        
      Vec<D> normal = mip.GetJacobianInverse() * grad;
      double len = L2Norm(normal);
      normal /= len;

      if (qn)
      {
        // cout << " normal = " << normal << endl;
        Vec<D> qnormal;
        qn->Evaluate(mip,qnormal);
        Vec<D> normal = mip.GetJacobianInverse() * qnormal;
        double len = L2Norm(normal);
        normal /= len;
        // cout << " normal = " << normal << endl;
        // getchar();
      }
        
      Vec<D> orig_point;
      for (int d = 0; d < D; ++d)
        orig_point(d) = ir[l](d);

      double goal_val = coef_lset_p1->Evaluate(mip);
      Vec<D> final_point;
      SearchCorrespondingPoint<D>(LsetEvaluator<D>(coef_lset_ho, eltrans),
                                  orig_point, goal_val, 
                                  trafo_of_normals, normal, false,
                                  final_point, lh);
      Vec<D> ref_dist = (final_point - orig_point);
      const double ref_dist_size = L2Norm(ref_dist);
      if ((max_deform >= 0.0) && (ref_dist_size > max_deform))
      {
        ref_dist *= max_deform / ref_dist_size; 
      }

        
      Vec<D> deform = mip.GetJacobian() * ref_dist;


      elvecmat += mip.GetWeight() * shape * Trans(deform);
    }      
  }


  template class ShiftIntegrator<2>;
  template class ShiftIntegrator<3>;

  static RegisterLinearFormIntegrator<ShiftIntegrator<2> > initpsistar2d ("shiftsource_2", 2, 2);
  static RegisterLinearFormIntegrator<ShiftIntegrator<3> > initpsistar3d ("shiftsource_2", 3, 2);
  static RegisterLinearFormIntegrator<ShiftIntegrator<2> > initpsistar2d3 ("shiftsource_3", 2, 3);
  static RegisterLinearFormIntegrator<ShiftIntegrator<3> > initpsistar3d3 ("shiftsource_3", 3, 3);
  static RegisterLinearFormIntegrator<ShiftIntegrator<2> > initpsistar2d4 ("shiftsource_4", 2, 4);
  static RegisterLinearFormIntegrator<ShiftIntegrator<3> > initpsistar3d4 ("shiftsource_4", 3, 4);
  static RegisterLinearFormIntegrator<ShiftIntegrator<2> > initpsistar2d5 ("shiftsource_5", 2, 5);
  static RegisterLinearFormIntegrator<ShiftIntegrator<3> > initpsistar3d5 ("shiftsource_5", 3, 5);
  static RegisterLinearFormIntegrator<ShiftIntegrator<2> > initpsistar2d6 ("shiftsource_6", 2, 6);
  static RegisterLinearFormIntegrator<ShiftIntegrator<3> > initpsistar3d6 ("shiftsource_6", 3, 6);


  template <int D>
  RestrictedMassIntegrator<D> :: RestrictedMassIntegrator (const Array<shared_ptr<CoefficientFunction>> & coeffs) : coef(coeffs[0]), coef_lset_p1(coeffs[1])
  {
    if (coeffs.Size() > 2)
    {
      std::cout << " asdfad " << std::endl;
      lower_lset_bound = coeffs[2]->EvaluateConst();
    }
    if (coeffs.Size() > 3)
    {
      std::cout << " asdfda " << std::endl;
      upper_lset_bound = coeffs[3]->EvaluateConst();
    }
  }

  template <int D>
  void RestrictedMassIntegrator<D> :: CalcElementMatrix (const FiniteElement & fel,
                                                         const ElementTransformation & eltrans,
                                                         FlatMatrix<double> elmat,
                                                         LocalHeap & lh) const
  {
    elmat = 0.0;
    const ScalarFiniteElement<D> & scafe = dynamic_cast<const ScalarFiniteElement<D> &>(fel);
      
    elmat = 0.0;

    ELEMENT_TYPE et = eltrans.GetElementType();
    bool has_neg = false;
    bool has_pos = false;
    for (int s = 0; s < ElementTopology::GetNVertices(et); ++s)
    {
      const double * v = ElementTopology::GetVertices(et)[s];
      IntegrationPoint ip(v[0],v[1],v[2]);
      MappedIntegrationPoint<D,D> mip(ip, eltrans);
      const double val = coef_lset_p1->Evaluate(mip);
      if (val==0.0) has_neg = has_pos = true;
      if (val > lower_lset_bound)
        has_pos = true;
      if (val < upper_lset_bound)
        has_neg = true;
    }
    if (!has_neg || !has_pos)
      return;
      
    FlatVector<> shape (scafe.GetNDof(),lh);
      
    IntegrationRule ir = SelectIntegrationRule (eltrans.GetElementType(), 2*scafe.Order());
    for (int l = 0 ; l < ir.GetNIP(); l++)
    {
      MappedIntegrationPoint<D,D> mip(ir[l], eltrans);
      const double coef_val = coef->Evaluate(mip);
      scafe.CalcShape(ir[l],shape);
      elmat += coef_val * mip.GetWeight() * shape * Trans(shape);
    }      
  }

  template class RestrictedMassIntegrator<2>;
  template class RestrictedMassIntegrator<3>;

  static RegisterBilinearFormIntegrator<RestrictedMassIntegrator<2> > initmassstar2d ("restrictedmass_2", 2, 2);
  static RegisterBilinearFormIntegrator<RestrictedMassIntegrator<3> > initmassstar3d ("restrictedmass_2", 3, 2);
  static RegisterBilinearFormIntegrator<RestrictedMassIntegrator<2> > initmassstar2d3 ("restrictedmass_3", 2, 3);
  static RegisterBilinearFormIntegrator<RestrictedMassIntegrator<3> > initmassstar3d3 ("restrictedmass_3", 3, 3);
  static RegisterBilinearFormIntegrator<RestrictedMassIntegrator<2> > initmassstar2d4 ("restrictedmass_4", 2, 4);
  static RegisterBilinearFormIntegrator<RestrictedMassIntegrator<3> > initmassstar3d4 ("restrictedmass_4", 3, 4);
  static RegisterBilinearFormIntegrator<RestrictedMassIntegrator<2> > initmassstar2d5 ("restrictedmass_5", 2, 5);
  static RegisterBilinearFormIntegrator<RestrictedMassIntegrator<3> > initmassstar3d5 ("restrictedmass_5", 3, 5);
  
}

