/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2012 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "outputFilterOutputControl.H"

// * * * * * * * * * * * * * Static Member Data  * * * * * * * * * * * * * * //

namespace Foam
{
    template<>
    const char* NamedEnum<outputFilterOutputControl::outputControls, 2>::
    names[] =
    {
        "timeStep",
        "outputTime"
    };
}

const Foam::NamedEnum<Foam::outputFilterOutputControl::outputControls, 2>
    Foam::outputFilterOutputControl::outputControlNames_;


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::outputFilterOutputControl::outputFilterOutputControl
(
    const Time& t,
    const dictionary& dict
)
:
    time_(t),
    outputControl_(ocTimeStep),
    outputInterval_(0),
    outputTimeLastDump_(0)
{
    read(dict);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::outputFilterOutputControl::~outputFilterOutputControl()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::outputFilterOutputControl::read(const dictionary& dict)
{
    if (dict.found("outputControl"))
    {
        outputControl_ = outputControlNames_.read(dict.lookup("outputControl"));
    }
    else
    {
        outputControl_ = ocTimeStep;
    }

    switch (outputControl_)
    {
        case ocTimeStep:
        {
            outputInterval_ = dict.lookupOrDefault<label>("outputInterval", 0);
            break;
        }

        case ocOutputTime:
        {
            outputInterval_ = dict.lookupOrDefault<label>("outputInterval", 1);
            break;
        }

        default:
        {
            // do nothing
            break;
        }
    }
}


bool Foam::outputFilterOutputControl::output()
{
    switch (outputControl_)
    {
        case ocTimeStep:
        {
            return
            (
                (outputInterval_ <= 1)
             || !(time_.timeIndex() % outputInterval_)
            );
            break;
        }

        case ocOutputTime:
        {
            if (time_.outputTime())
            {
                outputTimeLastDump_ ++;
                return !(outputTimeLastDump_ % outputInterval_);
            }
            break;
        }

        default:
        {
            // this error should not actually be possible
            FatalErrorIn("bool Foam::outputFilterOutputControl::output()")
                << "Undefined output control: "
                << outputControlNames_[outputControl_] << nl
                << abort(FatalError);
            break;
        }
    }

    return false;
}


// ************************************************************************* //
