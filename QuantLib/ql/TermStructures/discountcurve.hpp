
/*
  Copyright (C) 2002, 2003 Andre Louw.

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

/*! \file discountcurve.hpp
    \brief pre-bootstrapped discount factor structure
*/

#ifndef quantlib_discount_curve_h
#define quantlib_discount_curve_h

#include <ql/termstructure.hpp>
#include <ql/DayCounters/actual365.hpp>
#include <ql/Math/loglinearinterpolation.hpp>

namespace QuantLib {

    namespace TermStructures {

        //! Term structure based on loglinear interpolation of discount factors
        /*! Loglinear interpolation guarantees piecewise constant forward
            rates.

            Rates are assumed to be annual continuos compounding.
        */
        class DiscountCurve : public DiscountStructure {
          public:
            // constructor
            DiscountCurve(const Date& todaysDate,
                          const std::vector<Date>& dates,
                          const std::vector<DiscountFactor>& discounts,
                          const DayCounter& dayCounter = 
                              DayCounters::Actual365());

            DayCounter dayCounter() const;
            Date todaysDate() const {return todaysDate_; }
            Date referenceDate() const;
            const std::vector<Date>& dates() const;
            Date maxDate() const;
            const std::vector<Time>& times() const;
            Time maxTime() const;
          protected:
            DiscountFactor discountImpl(Time t, 
                                        bool extrapolate = false) const;
          private:
            Date todaysDate_;
            std::vector<Date> dates_;
            std::vector<DiscountFactor> discounts_;
            DayCounter dayCounter_;
            std::vector<Time> times_;
            typedef Math::LogLinearInterpolation <
                std::vector<Time>::const_iterator,
                std::vector<double>::const_iterator > DfInterpolation;
            Handle<DfInterpolation> interpolation_;
        };
       
        // inline definitions

        inline Date DiscountCurve::referenceDate() const {
            return dates_[0];
        }

        inline DayCounter DiscountCurve::dayCounter() const {
            return dayCounter_;
        }

        inline const std::vector<Date>& DiscountCurve::dates() const {
            return dates_;
        }

        inline Date DiscountCurve::maxDate() const {
            return dates_.back();
        }

        inline const std::vector<Time>& DiscountCurve::times() const {
            return times_;
        }

        inline Time DiscountCurve::maxTime() const {
            return times_.back();
        }

    }

}


#endif
