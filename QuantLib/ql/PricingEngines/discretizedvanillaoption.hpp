
/*
 Copyright (C) 2002, 2003 Sadruddin Rejeb

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file discretizedvanillaoption.hpp
    \brief discretized vanilla option
*/

#ifndef quantlib_discretized_vanilla_option_h
#define quantlib_discretized_vanilla_option_h

#include <ql/numericalmethod.hpp>
#include <ql/Lattices/bsmlattice.hpp>
#include <ql/Pricers/singleassetoption.hpp>
#include <ql/PricingEngines/vanillaengines.hpp>

namespace QuantLib {

    namespace PricingEngines {

        class DiscretizedVanillaOption : public DiscretizedAsset {
          public:
            DiscretizedVanillaOption(const Handle<NumericalMethod>& method,
                                     const VanillaOptionArguments& arguments)
            : DiscretizedAsset(method), arguments_(arguments) {}

            void reset(Size size);

            void adjustValues();

            void addTimes(std::list<Time>& times) const {
                for (Size i=0; i<arguments_.stoppingTimes.size(); i++)
                    times.push_back(arguments_.stoppingTimes[i]);
            }

          private:
            void applySpecificCondition();
            VanillaOptionArguments arguments_;
        };

    }

}


#endif
