#include "stdlib.h"
#include "graph.h"

void write_graph_color(uint16_t i, uint16_t* buff_draw, uint16_t* buff_clean, uint16_t color)
{
  if(i>0 && i < ILI9488_width()-0){ // clean old ray
    tft_draw_dot_line(i+1, buff_clean[i+1], i, buff_clean[i], 3, ILI9488_BLACK);
  }
  if(i>2 && i < ILI9488_width()-2){ // draw new ray
    tft_draw_dot_line(i, buff_draw[i], i-1, buff_draw[i-1], 3, color);
  }
  buff_clean[i]=buff_draw[i];
}

void tft_draw_dot_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t solid, uint16_t color)
{
  uint8_t DOT_SIZE = 8; // space between draw points

  int xerr=0,yerr=0,delta_y,delta_x,distance;
  int incx,incy,uCol,uRow;

  delta_x=x2-x1;
  delta_y=y2-y1;
  uRow=x1;
  uCol=y1;

  if(delta_y>0)
  {
    incx=DOT_SIZE;
  }
  else if(delta_y==0)
  {
    incx=0;
  }
  else
  {
    incx=-DOT_SIZE;
    delta_y=-delta_y;
  }

  if(delta_x>0)
  {
    incy=DOT_SIZE;
  }
  else if(delta_x==0)
  {
    incy=0;
  }
  else
  {
    incy=-DOT_SIZE;
    delta_x=-delta_x;
  }

  if( delta_y>delta_x)
  {
      distance=delta_y;
  }
  else
  {
    distance=delta_x;
  }

  for(uint16_t t=0;t<=distance+1;t+=DOT_SIZE )
  {
    ILI9488_fillRect(uRow,uCol, solid,solid, color);
    xerr+=delta_y ;
    yerr+=delta_x ;

    if(xerr>distance)
    {
      xerr-=distance;
      uCol+=incx;
    }
    if(yerr>distance)
    {
      yerr-=distance;
      uRow+=incy;
    }
  }
}

/*
** draw coordinate grid
*/
void write_grid(void)
{
  #define GRIDSIZE 8
  #define CELLSIZE 48
  #define CELLSIZE_HALF (CELLSIZE / 2)-16
  #define BEGIN_H_GRID 4
  #define BEGIN_V_GRID (BEGIN_H_GRID*2)

  #define WIDTH_H_GRID (480-8)
  #define WIDTH_V_GRID (480)
  #define END_V_GRID (156)
  #define END_H_GRID (156)

  for(uint16_t x = CELLSIZE_HALF;x<END_V_GRID;x+=GRIDSIZE)
    for(uint16_t i = BEGIN_V_GRID<<0; i<WIDTH_V_GRID; i+=CELLSIZE)
      ILI9488_fillRect(i, x, 2, 1, ILI9488_CYAN); // draw vertical dot line
  for(uint16_t x = CELLSIZE_HALF;x<END_H_GRID;x+=CELLSIZE)
    for(uint16_t i = BEGIN_H_GRID<<1; i<WIDTH_H_GRID; i+=GRIDSIZE)
      ILI9488_fillRect( i,  x,  2,  1,  ILI9488_LIGHTGREY); // draw horizontal dot line
}

