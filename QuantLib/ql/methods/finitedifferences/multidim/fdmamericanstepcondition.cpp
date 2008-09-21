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

/*! \file fdmamericanstepcondition.cpp
    \brief american step condition for multi dimensional problems
*/

#include <ql/methods/finitedifferences/multidim/fdmamericanstepcondition.hpp>

namespace QuantLib {

    FdmAmericanStepCondition::FdmAmericanStepCondition(
            const boost::shared_ptr<FdmMesher> & mesher,
            const boost::shared_ptr<FdmInnerValueCalculator> & calculator)
    : mesher_(mesher),
      calculator_(calculator) {
    }

    void FdmAmericanStepCondition::applyTo(Array& a, Time t) const {
        boost::shared_ptr<FdmLinearOpLayout> layout = mesher_->layout();
        const FdmLinearOpIterator endIter = layout->end();

        const Size dims = layout->dim().size();
        Array locations(dims);

        for (FdmLinearOpIterator iter = layout->begin(); iter != endIter;
            ++iter) {
            for (Size i=0; i < dims; ++i)
                locations[i] = mesher_->location(iter, i);

            if (calculator_->innerValue(locations) > a[iter.index()]) {
                a[iter.index()] = calculator_->innerValue(locations);
            }
        }
    }
}
