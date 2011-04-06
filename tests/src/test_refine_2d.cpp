/* 
 *    Copyright (C) 2010 Imperial College London and others.
 *    
 *    Please see the AUTHORS file in the main source directory for a full list
 *    of copyright holders.
 *
 *    Gerard Gorman
 *    Applied Modelling and Computation Group
 *    Department of Earth Science and Engineering
 *    Imperial College London
 *
 *    amcgsoftware@imperial.ac.uk
 *    
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation,
 *    version 2.1 of the License.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *    USA
 */
#include <iostream>
#include <vector>

#include <omp.h>

#include "Mesh.h"
#include "Surface.h"
#include "vtk_tools.h"
#include "MetricField.h"

#include "Refine.h"

using namespace std;

int main(int argc, char **argv){
  Mesh<double, int> *mesh=NULL;
  import_vtu("../data/box10x10.vtu", mesh);

  Surface<double, int> surface(*mesh);

  MetricField<double, int> metric_field(*mesh, surface);

  size_t NNodes = mesh->get_number_nodes();

  vector<double> psi(NNodes);
  for(size_t i=0;i<NNodes;i++)
    psi[i] = pow(mesh->get_coords(i)[0], 4) + pow(mesh->get_coords(i)[1], 4);
  
  metric_field.add_field(&(psi[0]), 0.01);
  metric_field.update_mesh();
  
  Refine<double, int> adapt(*mesh, surface);

  double start_tic = omp_get_wtime();
  adapt.refine(sqrt(2));
  std::cout<<"Refine time = "<<omp_get_wtime()-start_tic<<std::endl;

  std::map<int, int> active_vertex_map;
  mesh->defragment(&active_vertex_map);
  surface.defragment(&active_vertex_map);

  export_vtu("../data/test_refine_2d.vtu", mesh);
  export_vtu("../data/test_refine_2d_surface.vtu", &surface);

  delete mesh;

  std::cout<<"pass"<<std::endl;

  return 0;
}
