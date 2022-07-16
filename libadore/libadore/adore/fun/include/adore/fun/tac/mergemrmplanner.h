/********************************************************************************
 * Copyright (C) 2017-2020 German Aerospace Center (DLR). 
 * Eclipse ADORe, Automated Driving Open Research https://eclipse.org/adore
 *
 * This program and the accompanying materials are made available under the 
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0 
 *
 * Contributors: 
 *   Daniel Heß - initial API and implementation
 ********************************************************************************/

#pragma once
#include "mrmplanner.h"
#include "basicconstraintsandreferences.h"
#include <adore/view/alane.h>
#include <adore/params/ap_longitudinal_planner.h>
#include <adore/params/ap_lateral_planner.h>
#include <adore/params/ap_tactical_planner.h>
#include "lanechangeconstraintsandreferences.h"


namespace adore
{
  namespace fun
  {

    /**
     * @brief Plans minimum risk maneuver to cancel lane change
     */
    template<int K,int P>
    class MergeMRMPlanner:public MRMPlanner<K,P>
    {
      private:
        FollowCenterlineReference lateralReference_;
        LateralAccelerationReference lateralAccelerationReference_;/**< curvature compensation*/
        LateralJerkReference lateralJerkReference_;/**< lateral jerk compensation*/
        LateralOffsetConstraintLM lateralOffsetConstraint_left_;/**< constraint for staying in lane boundaries, which are selected according to gap state*/
        LateralOffsetConstraintLM lateralOffsetConstraint_right_;/**< constraint for staying in lane boundaries, which are selected according to gap state*/
        HeadingConstraint headingConstraintUB_;/**< constraint for deviation from road direction */
        HeadingConstraint headingConstraintLB_;/**< constraint for deviation from road direction */
        LateralAccelerationConstraint lateralAccelerationConstraintUB_;/**< constraint for ay*/
        LateralAccelerationConstraint lateralAccelerationConstraintLB_;/**< constraint for ay*/

      public:
        /**
         * Constructor.
         * Initializes references and constraints by linking them to required data abstraction views and parameters.
         * @param lfv view for lane changes
         * @param plat lateral planning parameters
         * @param pveh vehicle parameters
         * @param ptrajectory trajectory generation parameters
         */
        MergeMRMPlanner(adore::view::ALaneChangeView* lcv,
                                  adore::params::APLateralPlanner* plat,
                                  adore::params::APTacticalPlanner* ptac,
                                  adore::params::APVehicle* pveh,
                                  adore::params::APTrajectoryGeneration* ptrajectory)
              : MRMPlanner<K,P>(lcv->getSourceLane(),plat,pveh,ptrajectory),
                lateralReference_(lcv->getSourceLane()),
                lateralAccelerationReference_(lcv->getSourceLane()),
                lateralJerkReference_(lcv->getSourceLane()),
                lateralOffsetConstraint_left_(lcv,pveh,plat,ANominalConstraint::UB),
                lateralOffsetConstraint_right_(lcv,pveh,plat,ANominalConstraint::LB),
                headingConstraintUB_(plat,ANominalConstraint::UB),
                headingConstraintLB_(plat,ANominalConstraint::LB),
                lateralAccelerationConstraintUB_(lcv->getSourceLane(),plat,ANominalConstraint::UB),
                lateralAccelerationConstraintLB_(lcv->getSourceLane(),plat,ANominalConstraint::LB)
        {
          this->getOffsetSolver().getInformationSet().add(&lateralReference_);
          this->getOffsetSolver().getInformationSet().add(&lateralAccelerationReference_);
          this->getOffsetSolver().getInformationSet().add(&lateralJerkReference_);
          this->getOffsetSolver().getInformationSet().add(&lateralOffsetConstraint_left_);
          this->getOffsetSolver().getInformationSet().add(&lateralOffsetConstraint_right_);
          this->getOffsetSolver().getInformationSet().add(&headingConstraintUB_);
          this->getOffsetSolver().getInformationSet().add(&headingConstraintLB_);
          this->getOffsetSolver().getInformationSet().add(&lateralAccelerationConstraintUB_);
          this->getOffsetSolver().getInformationSet().add(&lateralAccelerationConstraintLB_);
        }

        void addConstraint(ANominalConstraint* constraint)
        {
          this->info_.add(constraint);
        }

        void addReference(ANominalReference* reference)
        {
          this->info_.add(reference);
        }
    };
  }
}