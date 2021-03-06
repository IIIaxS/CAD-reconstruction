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
//   Created by:          $Author:    daniel.baumgaertner@tum.de $
//   Date:                $Date:                   December 2016 $
//   Revision:            $Revision:                         0.0 $
//
// ==============================================================================

// ------------------------------------------------------------------------------
// System includes
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
// External includes
// ------------------------------------------------------------------------------
#include <boost/python.hpp>

// ------------------------------------------------------------------------------
// Project includes
// ------------------------------------------------------------------------------
#include "includes/define.h"
#include "processes/process.h"
#include "custom_python/add_custom_utilities_to_python.h"
#include "custom_utilities/optimization_utilities.h"
#include "custom_utilities/geometry_utilities.h"
#include "custom_utilities/vertex_morphing_mapper.h"
#include "custom_utilities/response_functions/strain_energy_response_function.h"
#include "custom_utilities/response_functions/mass_response_function.h"
#include "custom_utilities/test_function_utilities.h"
#include "custom_utilities/cad_reconstruction/cad_mapper.h"

// ==============================================================================

namespace Kratos
{

namespace Python
{


void  AddCustomUtilitiesToPython()
{
    using namespace boost::python;

    typedef UblasSpace<double, CompressedMatrix, Vector> SparseSpaceType;
    typedef UblasSpace<double, Matrix, Vector> LocalSpaceType;
    typedef LinearSolver<SparseSpaceType, LocalSpaceType > LinearSolverType;

    // ================================================================
    // For perfoming the mapping according to Vertex Morphing
    // ================================================================
    class_<VertexMorphingMapper, bases<Process> >("VertexMorphingMapper", init<ModelPart&, std::string, bool, double, const int>())
                        .def("compute_mapping_matrix", &VertexMorphingMapper::compute_mapping_matrix)
            .def("map_sensitivities_to_design_space", &VertexMorphingMapper::map_sensitivities_to_design_space)
            .def("map_design_update_to_geometry_space", &VertexMorphingMapper::map_design_update_to_geometry_space)
            ;

    // ========================================================================
    // For performing individual steps of an optimization algorithm
    // ========================================================================
    class_<OptimizationUtilities, bases<Process> >("OptimizationUtilities", init<ModelPart&, boost::python::dict, boost::python::dict, double, bool>())

            // ----------------------------------------------------------------
            // For running unconstrained descent methods
            // ----------------------------------------------------------------
            .def("compute_search_direction_steepest_descent", &OptimizationUtilities::compute_search_direction_steepest_descent)

            // ----------------------------------------------------------------
            // For running augmented Lagrange method
            // ----------------------------------------------------------------
            .def("initialize_augmented_lagrange", &OptimizationUtilities::initialize_augmented_lagrange)
            .def("compute_search_direction_augmented_lagrange", &OptimizationUtilities::compute_search_direction_augmented_lagrange)
            .def("udpate_augmented_lagrange_parameters", &OptimizationUtilities::udpate_augmented_lagrange_parameters)
            .def("get_penalty_fac", &OptimizationUtilities::get_penalty_fac)
            .def("get_lambda", &OptimizationUtilities::get_lambda)
            .def("get_value_of_augmented_lagrangian", &OptimizationUtilities::get_value_of_augmented_lagrangian)

            // ----------------------------------------------------------------
            // For running penalized projection method
            // ----------------------------------------------------------------
            .def("compute_projected_search_direction", &OptimizationUtilities::compute_projected_search_direction)
                        .def("correct_projected_search_direction", &OptimizationUtilities::correct_projected_search_direction)

            // ----------------------------------------------------------------
            // General optimization operations
            // ----------------------------------------------------------------
            .def("compute_design_update", &OptimizationUtilities::compute_design_update)
            ;

    // ========================================================================
    // For pre- and post-processing of geometry data
    // ========================================================================
    class_<GeometryUtilities, bases<Process> >("GeometryUtilities", init<ModelPart&>())
            .def("compute_unit_surface_normals", &GeometryUtilities::compute_unit_surface_normals)
            .def("project_grad_on_unit_surface_normal", &GeometryUtilities::project_grad_on_unit_surface_normal)
            .def("extract_surface_nodes", &GeometryUtilities::extract_surface_nodes)
            ;

    // ========================================================================
    // For calculations related to response functions
    // ========================================================================
    class_<StrainEnergyResponseFunction, bases<Process> >("StrainEnergyResponseFunction", init<ModelPart&, boost::python::dict>())
            .def("initialize", &StrainEnergyResponseFunction::initialize)
            .def("calculate_value", &StrainEnergyResponseFunction::calculate_value)
            .def("calculate_gradient", &StrainEnergyResponseFunction::calculate_gradient) 
            .def("get_value", &StrainEnergyResponseFunction::get_value)
            .def("get_initial_value", &StrainEnergyResponseFunction::get_initial_value)  
            .def("get_gradient", &StrainEnergyResponseFunction::get_gradient)                              
            ; 
    class_<MassResponseFunction, bases<Process> >("MassResponseFunction", init<ModelPart&, boost::python::dict>())
            .def("initialize", &MassResponseFunction::initialize)
            .def("calculate_value", &MassResponseFunction::calculate_value)
            .def("calculate_gradient", &MassResponseFunction::calculate_gradient)  
            .def("get_value", &MassResponseFunction::get_value)
            .def("get_initial_value", &MassResponseFunction::get_initial_value) 
            .def("get_gradient", &MassResponseFunction::get_gradient)                              
            ;             

    // ========================================================================
    // For testing purposes
    // ========================================================================
    class_<FunctionTester, bases<Process> >("FunctionTester", init<ModelPart&>())
            .def("test_function", &FunctionTester::test_function)
            ;         

    // ========================================================================
    // For CAD reconstruction
    // ========================================================================
    class_<CADMapper, bases<Process> >("CADMapper", init<ModelPart&, boost::python::dict, unsigned int, LinearSolverType::Pointer>())
            .def("compute_mapping_matrix", &CADMapper::compute_mapping_matrix)
            .def("map_to_cad_space", &CADMapper::map_to_cad_space)
            .def("compute_point", &CADMapper::compute_point)
            .def("output_control_point_displacements", &CADMapper::output_control_point_displacements)
            ;            
}


}  // namespace Python.

} // Namespace Kratos

