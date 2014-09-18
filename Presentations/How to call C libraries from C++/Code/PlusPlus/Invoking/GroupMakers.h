//
//  GroupMakers.h
//  PlusPlus
//
//  Created by Lisa Lippincott on 8/30/14.
//  Released into the public domain by Lisa Lippincott, 2014.
//

#ifndef PLUSPLUS_GROUPMAKERS_H
#define PLUSPLUS_GROUPMAKERS_H

#include "Returned.h"
#include "Thrown.h"
#include "Checked.h"
#include "In.h"
#include "Out.h"
#include "InOut.h"
#include "NotPassed.h"
#include "Result.h"

/*
    The namespace PlusPlus::GroupMakers contains all the names needed for 
    creating the basic pararameter and result groups, gathered together 
    for use in a using-directive.
*/

namespace PlusPlus
   {
    namespace GroupMakers
       {
        // Groups
            using PlusPlus::In;
            
            using PlusPlus::Out;
            using PlusPlus::OutFailureFlag;
            using PlusPlus::OutSuccessFlag;
            using PlusPlus::OutError;
            
            using PlusPlus::InOut;
            
            using PlusPlus::NotPassed;
            using PlusPlus::ExceptionToThrow;
            
            using PlusPlus::Result;
            using PlusPlus::NoResult;
            using PlusPlus::FailureFlagResult;
            using PlusPlus::SuccessFlagResult;
            using PlusPlus::ErrorResult;
            
        // Modifiers
            using PlusPlus::FailsWhen;
            using PlusPlus::SucceedsWhen;
            using PlusPlus::FailsWhenTrue;
            using PlusPlus::FailsWhenFalse;
            
            using PlusPlus::Returned;
            using PlusPlus::NotReturned;
            
            using PlusPlus::Thrown;
            using PlusPlus::NotThrown;
       }
   }

#endif
