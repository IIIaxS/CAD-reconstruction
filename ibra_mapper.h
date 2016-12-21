// ==============================================================================
/*
 KratosShapeOptimizationApplication
 A library based on:
 Kratos
 A General Purpose Software for Multi-Physics Finite Element Analysis
 (Released on march 05, 2007).

 Copyright (c) 2016: Daniel Baumgaertner
                     daniel.baumgaertner@tum.de
                     Chair of Structural Analysis
                     Technische Universitaet Muenchen
                     Arcisstrasse 21 80333 Munich, Germany

 Permission is hereby granted, free  of charge, to any person obtaining
 a  copy  of this  software  and  associated  documentation files  (the
 "Software"), to  deal in  the Software without  restriction, including
 without limitation  the rights to  use, copy, modify,  merge, publish,
 distribute,  sublicense and/or  sell copies  of the  Software,  and to
 permit persons to whom the Software  is furnished to do so, subject to
 the following condition:

 Distribution of this code for  any  commercial purpose  is permissible
 ONLY BY DIRECT ARRANGEMENT WITH THE COPYRIGHT OWNERS.

 The  above  copyright  notice  and  this permission  notice  shall  be
 included in all copies or substantial portions of the Software.

 THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
 EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT  SHALL THE AUTHORS OR COPYRIGHT HOLDERS  BE LIABLE FOR ANY
 CLAIM, DAMAGES OR  OTHER LIABILITY, WHETHER IN AN  ACTION OF CONTRACT,
 TORT  OR OTHERWISE, ARISING  FROM, OUT  OF OR  IN CONNECTION  WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//==============================================================================
//
//   Project Name:        KratosShape                            $
//   Created by:          $Author:    giovanni.filomeno@tum.de   $
//   Last modified by:    $Co-Author: giovanni.filomeno@tum.de   $
//   Date:                $Date:                      Decem 2016 $
//   Revision:            $Revision:                         0.0 $
//
// ==============================================================================

#ifndef IBRA_MAPPER_H
#define IBRA_MAPPER_H

// ------------------------------------------------------------------------------
// System includes
// ------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <algorithm>
#include <Python.h>

// ------------------------------------------------------------------------------
// External includes
// ------------------------------------------------------------------------------
#include <boost/python.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

// ------------------------------------------------------------------------------
// Project includes
// ------------------------------------------------------------------------------
#include "../../kratos/includes/define.h"
#include "../../kratos/processes/process.h"
#include "../../kratos/includes/node.h"
#include "../../kratos/includes/element.h"
#include "../../kratos/includes/model_part.h"
#include "../../kratos/includes/kratos_flags.h"
#include "shape_optimization_application.h"
#include "patch.h"
#include "control_point.hpp"
// ==============================================================================

namespace Kratos
{
class IBRAMapper
{
public:
    ///@name Type Definitions
    ///@{


    // ==========================================================================
    // Type definitions for linear algebra including sparse systems
    // ==========================================================================
    typedef UblasSpace<double, CompressedMatrix, Vector> SparseSpaceType;
    typedef typename SparseSpaceType::MatrixType SparseMatrixType;
    typedef typename SparseSpaceType::VectorType VectorType;
    typedef std::vector<double> DoubleVector;
    typedef boost::python::extract<double> takeDouble;
    typedef boost::python::extract<int> takeInt;
    typedef boost::python::extract<bool> takeBool;
    typedef std::vector<ControlPoint> controlPointVcr;
    typedef std::vector<int> IntVector;
//    static PyObject *;

    /// Pointer definition of IBRAMapper
    KRATOS_CLASS_POINTER_DEFINITION(IBRAMapper);

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    IBRAMapper( ModelPart& model_part,
    		boost::python::dict self)
		: mr_model_part(model_part),
          myPythondict(self)
    {
    }

    /// Destructor.
    virtual ~IBRAMapper()
    {
    }

    void compute_mapping_matrix()
    {
    	// loop over faces
    	for(int i=0; i < boost::python::len(myPythondict["faces"]); i++)
    	{
    		std::cout << "face: " << i << std::endl;

    	}
    }

    boost::python::list compute_point(unsigned int patch_id, double u, double v)
    {
    	boost::python::list point;
    	double x = 0, y = 0, z = 0;
    	patches[ patch_id ].S( x, y, z, u, v);

    	point.append(x);
    	point.append(y);
    	point.append(z);

    	KRATOS_WATCH("!!!!!!!!!!!!!!!!!!!!!!!!!!")

    	return point;
    }


    // ==============================================================================
    void initialize_patches()
    {
    	// loop over faces
    	    	for(int i=0; i < boost::python::len(myPythondict["faces"]); i++)
    	    	{
//    	    		std::cout << "face: " << i << std::endl;

    	    		// each face is a patch: myPythondict["faces"][i]

    	    		// 1. initialize "ingredients" ==================================
    	    		DoubleVector knot_vector_u;
    	    		DoubleVector knot_vector_v;
    	    		int p;
    	    		int q;
    	    		controlPointVcr control_points;

    	    		// 2. create "ingredients" ======================================
    	    		// fill in knot_vector_u
    	    		for(int u_idx = 0;
    	    				u_idx < boost::python::len(myPythondict["faces"][i]["surface"][0]["knot_vectors"][0]);
    	    				u_idx ++)
    	    		{
    	    			// read knot
    	    			double knot = takeDouble(myPythondict["faces"][i]["surface"][0]["knot_vectors"][0][u_idx]);
    	    			knot_vector_u.push_back( knot );
    	    		}


//    	    		for( auto entris : knot_vector_u)
//    	    		{
//    	    			std::cout << "my test: " << entris << std::endl;
//    	    		}
    	    		// fill in knot_vector_v

    	    		for(int v_idx = 0;
    	    			v_idx < boost::python::len(myPythondict["faces"][i]["surface"][0]["knot_vectors"][1]);
    	    		    v_idx ++)
    	    		{
    	    			// read knot
    	    			double knot = takeDouble(myPythondict["faces"][i]["surface"][0]["knot_vectors"][1][v_idx]);
    	    			knot_vector_v.push_back( knot );
    	    		}

//    	    		for( auto entris : knot_vector_v)
//    	    		{
//    	    			std::cout << "my test: " << entris << std::endl;
//    	    		}
    	    		// get p and q
    	    		p = takeInt(myPythondict["faces"][i]["surface"][0]["degrees"][0]);
    	    		q = takeInt(myPythondict["faces"][i]["surface"][0]["degrees"][1]);

    	    		// fill in control_points

    	    		for(int cp_idx = 0;
    	    				cp_idx < boost::python::len(myPythondict["faces"][i]["surface"][0]["control_points"]);
    	    				cp_idx ++)
    	    		{
//    	    			std::cout << cp_idx << std::endl;

    	    			int ID = takeInt( myPythondict["faces"][i]["surface"][0]["control_points"][cp_idx][0] );
    	    			double x = takeDouble( myPythondict["faces"][i]["surface"][0]["control_points"][cp_idx][1][0] );
    	    			double y = takeDouble( myPythondict["faces"][i]["surface"][0]["control_points"][cp_idx][1][1] );;
    	    			double z = takeDouble( myPythondict["faces"][i]["surface"][0]["control_points"][cp_idx][1][2] );;
    	    			double w = takeDouble( myPythondict["faces"][i]["surface"][0]["control_points"][cp_idx][1][3] );;

    	    			// read knot
    	    			ControlPoint myControlPoint( x, y, z, w, ID);
    	    			control_points.push_back( myControlPoint );

    	    		}

//    	    		for( auto entries : control_points)
//    	    		{
//    	    			std::cout << "ID: " << entries.getID() << std::endl;
//    	    			std::cout << "X: " << entries.getX() << std::endl;
//    	    			std::cout << "Y: " << entries.getY() << std::endl;
//    	    			std::cout << "Z: " << entries.getZ() << std::endl;
//    	    			std::cout << "W: " << entries.getWeight() << std::endl;
//    	    		}


    	    		// 3. create patch  =============================================
    	    		Patch patch (knot_vector_u, knot_vector_v, p, q, control_points);
    	    		patches.push_back(patch);
    	    	}
//        KRATOS_TRY;

//        VectorType knot_vector = ZeroVector(3);

//        double test;
//
//        DoubleVector coordinates;
//        controlPointVcr myControlPoint;
//        IntVector degree;
//        DoubleVector knotVector;
//
//        for( int k = 0; k < 9; k++)
//        {
//
//        	std::cout << "checkmark 100" << std::endl;
//
//        	int list_length = boost::python::len(myPythondict["faces"]);

//        	std::cout << "list_length = " << list_length << std::endl;

//        	takeInt ID(myPythondict["faces"][0]["surface"][0]["control_points"][k][0]);
//
//        	for(int i = 0; i < 3; i++)
//        	{
//        		std::cout << "Porco dio nel secondo for" << std::endl;
//        	  	double myCoord = takeDouble (myPythondict["faces"][0]["surface"][0]["control_points"][0][k][1][i]);
//        	   	coordinates.push_back( myCoord );
//        	}
//
//        	double weight = takeDouble (myPythondict["faces"][0]["surface"][0]["control_points"][0][k][1][3]);
//
//        	controlPoint controlPointFromJson( coordinates, weight, ID);
//        	myControlPoint.push_back( controlPointFromJson );
//
//        }

//        for(int k = 0; k < 2; k++)
//        {
//        	int deg = takeInt (myPythondict["faces"][0]["surface"][0]["degree"][k]);
//        	degree.push_back( deg );
//        }

//        bool is_rational = takeBool (myPythondict["faces"][0]["surface"][0]["is_rational"]);
//        bool is_trimmed = takeBool (myPythondict["faces"][0]["surface"][0]["is_trimmed"]);

//        for(int i = 0; i < 2; i++)
//        {
//        	for(int k = 0; k < 6; k++)
//        	{
//        		double knot = takeDouble (myPythondict["faces"][0]["surface"][0]["knot_vectors"][i][k]);
//        		knotVector.push_back( knot );
//        		std::cout << "Knot " << knot << std::endl;
//        	}
//        }


//        double test = boost::python::extract<double> (myPythondict["faces"]);
//
//        std::cout<<"Test1 " << test << std::endl;


//        int i=0;
//        while (i < myPythondict["knot_vector"].attr("length") )
//       {
//        	boost::python::extract<double> test2 (myPythondict["knot_vector"][i]);
//        	std::cout <<"vector test " << test2 << std::endl;
//        	i++;
//       }

//        boost::python::extract<const double *> test2 (myPythondict["knot_vector"]);
//
//        std::cout<<"vector test " << test2 << std::endl;




//        string test2;
//
//        boost::python::extract<const char*> test2(myPythondict["loop"]);
//
//        std::cout<<"Test2 " << test2 << std::endl;
//
//        boost::python::extract<double> test3(myPythondict["knot_vector"][0]);
//        std::cout << "Test3 " << test3 << std::endl;
//
//        boost::python::extract<Vec2&> test4(myPythondict["knot_vector"]);
//        std::cout<<"Test4 " << test4 << std::endl;
//        KRATOS_WATCH(test4);

//        KRATOS_WATCH(knot_vector);

//        KRATOS_CATCH("");
    }

//    evaluat

//    void Surface( int& i, double& u, double& v )
    void Surface()
    {
    	int i=1;
    	double u=5.0;
    	double v=5.0;
    	double x = 0, y = 0, z = 0;
    	patches[ i-1 ].S( x, y, z, u, v);
    	std::cout<<"My results: "  << x << " " << y << " " << z << std::endl;

//    	int i=1;
    	u=2.5;
    	v=2.5;
//    	double x = 0, y = 0, z = 0;
    	patches[ i-1 ].S( x, y, z, u, v);
    	std::cout<<"My results: "  << x << " " << y << " " << z << std::endl;


//    	int i=1;
    	u=3.75;
    	v=3.75;
//    	double x = 0, y = 0, z = 0;
    	patches[ i-1 ].S( x, y, z, u, v);
    	std::cout<<"My results: "  << x << " " << y << " " << z << std::endl;


//    	int i=1;
    	u=5.0;
    	v=0;
//    	double x = 0, y = 0, z = 0;
    	patches[ i-1 ].S( x, y, z, u, v);
    	std::cout<<"My results: "  << x << " " << y << " " << z << std::endl;

    }
    // ==============================================================================

    /// Turn back information as a string.
    virtual std::string Info() const
    {
        return "IBRAMapper";
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const
    {
        rOStream << "IBRAMapper";
    }

    /// Print object's data.
    virtual void PrintData(std::ostream& rOStream) const
    {
    }


private:
    // ==============================================================================
    // Initialized by class constructor
    // ==============================================================================
    ModelPart& mr_model_part;
    boost::python::dict myPythondict;

    // ==============================================================================
    // General working arrays
    // ==============================================================================
    std::vector< Patch > patches;
    SparseMatrixType m_mapping_matrix;

    /// Assignment operator.
//      IBRAMapper& operator=(IBRAMapper const& rOther);

    /// Copy constructor.
//      IBRAMapper(IBRAMapper const& rOther);

}; // Class IBRAMapper
}  // namespace Kratos.

#endif // IBRA_MAPPER_H
