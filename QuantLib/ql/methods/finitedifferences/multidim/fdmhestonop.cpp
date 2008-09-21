/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2008 Andreas Gaida
 Copyright (C) 2008 Ralph Schreyer
 Copyright (C) 2008 Klaus Spanderen

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/


/*! \file fdmhestonop.cpp
    \brief linear operator to model the Heston pde
*/

#include <ql/methods/finitedifferences/multidim/fdmhestonop.hpp>
#include <ql/methods/finitedifferences/multidim/secondderivativeop.hpp>
#include <ql/methods/finitedifferences/multidim/secondordermixedderivativeop.hpp>

namespace QuantLib {

    FdmHestonEquityPart::FdmHestonEquityPart(
        const boost::shared_ptr<FdmMesher>& mesher,
        const boost::shared_ptr<YieldTermStructure>& rTS,
        const boost::shared_ptr<YieldTermStructure>& qTS)
    : varianceValues_(0.5*mesher->locations(1)),
      dxMap_ (FirstDerivativeOp(0, mesher)),
      dxxMap_(SecondDerivativeOp(0, mesher).mult(0.5*mesher->locations(1))),
      mapT_   (0, mesher),
      mesher_(mesher),
      rTS_(rTS),
      qTS_(qTS) {
            
        // on the boundary s_min and s_max the second derivative 
        // d^2V/dS^2 is zero and due to Ito's Lemma the variance term
        // in the drift should vanish.
        boost::shared_ptr<FdmLinearOpLayout> layout = mesher_->layout();
        FdmLinearOpIterator endIter = layout->end();
        for (FdmLinearOpIterator iter = layout->begin(); iter != endIter; 
            ++iter) {
            if (   iter.coordinates()[0] == 0 
                || iter.coordinates()[0] == layout->dim()[0]-1) {
                varianceValues_[iter.index()] = 0.0;
            }   
        }
        volatilityValues_ = Sqrt(2*varianceValues_);
    }

    void FdmHestonEquityPart::setTime(Time t1, Time t2) {
        const Real r = rTS_->forwardRate(t1, t2, Continuous);
        const Real q = qTS_->forwardRate(t1, t2, Continuous);

        mapT_.axpyb(r - q - varianceValues_, dxMap_, 
                    dxxMap_, Array(1, -0.5*r));
    }

    const TripleBandLinearOp& FdmHestonEquityPart::getMap() const {
        return mapT_;
    }

    FdmHestonVariancePart::FdmHestonVariancePart(
        const boost::shared_ptr<FdmMesher>& mesher,
        const boost::shared_ptr<YieldTermStructure>& rTS,
        Real sigma, Real kappa, Real theta)
    : dyMap_(SecondDerivativeOp(1, mesher)
                .mult(0.5*sigma*sigma*mesher->locations(1))
             .add(FirstDerivativeOp(1, mesher)
                  .mult(kappa*(theta - mesher->locations(1))))),
      mapT_(1, mesher),
      rTS_(rTS) {
    }

    void FdmHestonVariancePart::setTime(Time t1, Time t2) {
        const Real r = rTS_->forwardRate(t1, t2, Continuous);
        mapT_.axpyb(Array(), dyMap_, dyMap_, Array(1,-0.5*r));
    }

    const TripleBandLinearOp& FdmHestonVariancePart::getMap() const {
        return mapT_;
    }

    FdmHestonOp::FdmHestonOp(
        const boost::shared_ptr<FdmMesher>& mesher,
        const boost::shared_ptr<HestonProcess> & hestonProcess) 
    : v0_(hestonProcess->v0()),
      kappa_(hestonProcess->kappa()),
      theta_(hestonProcess->theta()),
      sigma_(hestonProcess->sigma()),
      rho_  (hestonProcess->rho()),
      rTS_  (hestonProcess->riskFreeRate().currentLink()),
      correlationMap_(SecondOrderMixedDerivativeOp(0, 1, mesher)
                        .mult(rho_*sigma_*mesher->locations(1))),
      dyMap_(mesher, rTS_,
             sigma_, kappa_, theta_),
      dxMap_(mesher, 
             rTS_, hestonProcess->dividendYield().currentLink()) { 
    }


    void FdmHestonOp::setTime(Time t1, Time t2) {
        dxMap_.setTime(t1, t2);
        dyMap_.setTime(t1, t2);
    }

    Size FdmHestonOp::size() const {
        return 2;
    }

    Disposable<Array> FdmHestonOp::apply(const Array& u) const {
        return dyMap_.getMap().apply(u) + dxMap_.getMap().apply(u) 
              + correlationMap_.apply(u);
    }

    Disposable<Array> FdmHestonOp::apply_direction(Size direction,
                                                    const Array& r) const {
        if (direction == 0)
            return dxMap_.getMap().apply(r);
        else if (direction == 1)
            return dyMap_.getMap().apply(r);
        else 
            QL_FAIL("direction too large");
    }

    Disposable<Array> FdmHestonOp::apply_mixed(const Array& r) const {
        return correlationMap_.apply(r);
    }

    Disposable<Array> FdmHestonOp::solve_splitting(Size direction,
                                                const Array& r, Real a) const {
        if (direction == 0) {
            return dxMap_.getMap().solve_splitting(r, a, 1.0);
        }
        else if (direction == 1) {
            return dyMap_.getMap().solve_splitting(r, a, 1.0);
        }
        else
            QL_FAIL("direction too large");
    }
}
