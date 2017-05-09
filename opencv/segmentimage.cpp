/* 
Copyright (C) 2006 Pedro Felzenszwalb 
 
This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version. 
 
This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU General Public License for more details. 
 
You should have received a copy of the GNU General Public License 
along with this program; if not, write to the Free Software 
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
*/  
  
  
  
#include <cstdlib>  
  
//#include "filter.h"  
#include "segmentgraph.h"  
#include "segmentimage.h"  
#include "time.h"  
  
// random color  
rgb random_rgb(){   
  rgb c;  
  double r;  
  srand(unsigned(time(NULL)));  
  c.r = rand()%256;  
  c.g = rand()%256;  
  c.b = rand()%256;  
  
  return c;  
}  
  
//// dissimilarity measure between pixels  
//static inline float diff(image<float> *r, image<float> *g, image<float> *b,  
//           int x1, int y1, int x2, int y2)   
//{  
//  return sqrt(square(imRef(r, x1, y1)-imRef(r, x2, y2)) +  
//        square(imRef(g, x1, y1)-imRef(g, x2, y2)) +  
//        square(imRef(b, x1, y1)-imRef(b, x2, y2)));  
//}  
  
/* 
 * Segment an image 
 * 
 * Returns a color image representing the segmentation. 
 * 
 * im: image to segment. 
 * sigma: to smooth the image. 
 * c: constant for treshold function. 
 * min_size: minimum component size (enforced by post-processing stage). 
 * num_ccs: number of connected components in the segmentation. 
 */  
Mat segment_image(Mat &img, float sigma, float c, int min_size,  
              int *num_ccs)   
{  
  int width = img.size().width;  
  int height = img.size().height;  
  //Mat img_(height, width, CV_8UC3); //using u8 will produce too many component  
  Mat img_(height, width, CV_32FC3);  
  
  // gaussian blur  
  int kSize = cvCeil(4*sigma)+1;  
  GaussianBlur(img, img_, Size(kSize, kSize), sigma, sigma);  
  
  // build graph  
  edge *edges = new edge[width*height*4];  
  int num = 0;  
  for (int y = 0; y < height; y++) {  
    for (int x = 0; x < width; x++) {  
      if (x < width-1) //right  
      {  
        edges[num].a = y * width + x;  
        edges[num].b = y * width + (x+1);  
  
  
        //edges[num].w = diff(rgbs, x, y, x + 1, y);  
        //edges[num].w = norm(img_.at<Vec3b>(y, x),img_.at<Vec3b>(y,x + 1));//by deafualt,normType=NORM_L2  
        edges[num].w = norm(img_.at<Vec3f>(y, x), img_.at<Vec3f>(y, x + 1));//by deafualt,normType=NORM_L2  
        num++;  
      }  
  
      if (y < height-1)//bottom   
      {  
        edges[num].a = y * width + x;  
        edges[num].b = (y+1) * width + x;  
  
        //edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x, y+1);  
        edges[num].w = norm(img_.at<Vec3f>(y, x), img_.at<Vec3f>(y + 1, x));  
        num++;  
      }  
  
      if ((x < width-1) && (y < height-1)) {//bottom right  
        edges[num].a = y * width + x;  
        edges[num].b = (y+1) * width + (x+1);  
  
        //edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y+1);  
        edges[num].w = norm(img_.at<Vec3f>(y, x), img_.at<Vec3f>(y + 1, x + 1));  
  
        num++;  
      }  
  
      if ((x < width-1) && (y > 0))//up right   
      {  
        edges[num].a = y * width + x;// find the component id to which they are subject    
        edges[num].b = (y-1) * width + (x+1);  
        //edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y-1);  
        edges[num].w = norm(img_.at<Vec3f>(y, x), img_.at<Vec3f>(y - 1, x + 1));  
  
        num++;  
      }  
    }  
  }  
  
  // segment the graph to sub graphs  
  /* 
  * width*height: component number before mergence  
  * num :         component number after mergence  
  * c:            correspond to the k in fomula (5) 
  */  
  universe *u = segment_graph(width*height, num, edges, c);  
  // post process small components  
  
  for (int i = 0; i < num; i++) {  
    int a = u->find(edges[i].a);  
    int b = u->find(edges[i].b);  
    if ((a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size)))  
      u->join(a, b); // as we have ranked the edges by weights so ,  
                     // if the compent'size is less than the threshold it will merge with the most similar component    
  }  
  delete [] edges;  
  *num_ccs = u->num_sets();  
  
  Mat output(height,width,CV_8UC3);  
  
  // pick random colors for each component  
  //rgb *colors = new rgb[width*height];  
  //for (int i = 0; i < width*height; i++)  
  //  colors[i] = random_rgb();  
  Mat colors(1, num, CV_8UC3);  
  randu(colors, Scalar::all(0), Scalar::all(255));  
    
  for (int y = 0; y < height; y++)   
  {  
    for (int x = 0; x < width; x++)   
    {  
      int comp = u->find(y * width + x);  
      //if(u->elts[comp].rank>2)  
      output.at<Vec3b>(y, x) = colors.at<Vec3b>(0,comp);  
  
    }  
  }     
  delete u;  
  return output;  
}