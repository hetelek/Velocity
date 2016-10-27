/*
 * NightCharts
 * Copyright (C) 2010 by Alexander A. Avdonin, Artem N. Ivanov / ITGears Co.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, Write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact gordos.kund@gmail.com with any questions on this license.
 */
#include "nightcharts.h"
#include <QGraphicsScene>

Nightcharts::Nightcharts()//QPainter *painter)

{
    font.setFamily("verdana");
    font.setPixelSize(15);
    //painter = painter;
    //painter->setFont(font);
    ctype = Nightcharts::Dpie;
    cltype = Nightcharts::Vertical;
    cX = 0;
    cY = 0;
    cW = 100;
    cH = 100;
    lX = cX+cW+20;
    lY = cY;
    shadows = true;
}
Nightcharts::~Nightcharts()
{
    pieces.clear();
}

int Nightcharts::addPiece(QString name,Qt::GlobalColor color,float Percentage)
{
    this->nPiece++;

    pieceNC piece;
    piece.addName(name);
    piece.setColor(color);
    piece.setPerc(Percentage);
    pieces.append(piece);

    return 0;
}
int Nightcharts::addPiece(QString name, QColor color, float Percentage)
{
    this->nPiece++;
    pieceNC piece;
    piece.addName(name);
    piece.setColor(color);
    piece.setPerc(Percentage);
    pieces.append(piece);

    return 0;
}
int Nightcharts::setCords(double x, double y, double w, double h)
{
    this->cX = x;
    this->cY = y;
    this->cW = w;
    this->cH = h;
    this->lX = cX+cW+20;
    this->lY = cY;

    return 0;
}
int Nightcharts::setLegendCords(double x, double y)
{
    this->lX = x;
    this->lY = y;

    return 0;
}

int Nightcharts::setType(Nightcharts::type t)
{
    this->ctype = t;

    return 0;
}

int Nightcharts::setLegendType(Nightcharts::legend_type t)
{
    this->cltype = t;

    return 0;
}
int Nightcharts::setFont(QFont f)
{
    this->font = f;

    return 0;
}

int Nightcharts::setShadows(bool ok)
{
    this->shadows = ok;

    return 0;
}

int Nightcharts::draw(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    if (this->ctype==Nightcharts::Pie)
    {
      pW = 0;
      double pdegree = 0;

      //Options
      QLinearGradient gradient(cX+0.5*cW,cY,cX+0.5*cW,cY+cH*2.5);
      gradient.setColorAt(1,Qt::black);


      //Draw
      //pdegree = (360/100)*pieces[i].pPerc;
      if (shadows)
      {
          double sumangle = 0;
          for (int i=0;i<pieces.size();i++)
          {
              sumangle += 3.6*pieces[i].pPerc;
          }
          painter->setBrush(Qt::darkGray);
          painter->drawPie(cX,cY+pW+5,cW,cH,palpha*16,sumangle*16);
      }

      QPen pen;
      pen.setWidth(2);

      for (int i=0;i<pieces.size();i++)
      {
        gradient.setColorAt(0,pieces[i].rgbColor);
        painter->setBrush(gradient);
        pen.setColor(pieces[i].rgbColor);
        painter->setPen(pen);
        pdegree = 3.6*pieces[i].pPerc;
        painter->drawPie(cX,cY,cW,cH,palpha*16,pdegree*16);
        palpha += pdegree;
      }
    }
    else if (this->ctype==Nightcharts::Dpie)
    {
        pW = 50;
        double pdegree = 0;
        QPointF p;

        QLinearGradient gradient(cX-0.5*cW,cY+cH/2,cX+1.5*cW,cY+cH/2);
        gradient.setColorAt(0,Qt::black);
        gradient.setColorAt(1,Qt::white);
        QLinearGradient gradient_side(cX,cY+cH,cX+cW,cY+cH);
        gradient_side.setColorAt(0,Qt::black);

        double sumangle = 0;
        for (int i=0;i<pieces.size();i++)
        {
            sumangle += 3.6*pieces[i].pPerc;
        }
        if (shadows)
        {
            painter->setBrush(Qt::darkGray);
            painter->drawPie(cX,cY+pW+5,cW,cH,palpha*16,sumangle*16);
        }
        int q = GetQuater(palpha+sumangle);

        if (q ==2 || q==3)
        {
            QPointF p = GetPoint(palpha+sumangle);
            QPointF points[4] =
            {
                QPointF(p.x(),p.y()),
                QPointF(p.x(),p.y()+pW),
                QPointF(cX+cW/2,cY+cH/2+pW),
                QPointF(cX+cW/2,cY+cH/2)
            };
            gradient_side.setColorAt(1,pieces[pieces.size()-1].rgbColor);
            painter->setBrush(gradient_side);
            painter->drawPolygon(points,4);
        }
        p = GetPoint(palpha);
        q = GetQuater(palpha);
        if (q ==1 || q==4)
        {
            QPointF points[4] =
            {
                QPointF(p.x(),p.y()),
                QPointF(p.x(),p.y()+pW),
                QPointF(cX+cW/2,cY+cH/2+pW),
                QPointF(cX+cW/2,cY+cH/2)
            };
            gradient_side.setColorAt(1,pieces[0].rgbColor);
            painter->setBrush(gradient_side);
            painter->drawPolygon(points,4);
        }

        for (int i=0;i<pieces.size();i++)
        {
          gradient.setColorAt(0.5,pieces[i].rgbColor);
          painter->setBrush(gradient);
          pdegree = 3.6*pieces[i].pPerc;
          painter->drawPie(cX,cY,cW,cH,palpha*16,pdegree*16);

          double a_ = Angle360(palpha);
          int q_ = GetQuater(palpha);

          palpha += pdegree;

          double a = Angle360(palpha);
          int q = GetQuater(palpha);

          QPainterPath path;
          p = GetPoint(palpha);

          if((q == 3 || q == 4) && (q_ == 3 || q_ == 4))
          {
              // 1)
              if (a>a_)
              {
                  QPointF p_old = GetPoint(palpha-pdegree);
                  path.moveTo(p_old.x()-1,p_old.y());
                  path.arcTo(cX,cY,cW,cH,palpha-pdegree,pdegree);
                  path.lineTo(p.x(),p.y()+pW);
                  path.arcTo(cX,cY+pW,cW,cH,palpha,-pdegree);
              }
              // 2)
              else
              {
                  path.moveTo(cX,cY+cH/2);
                  path.arcTo(cX,cY,cW,cH,180,Angle360(palpha)-180);
                  path.lineTo(p.x(),p.y()+pW);
                  path.arcTo(cX,cY+pW,cW,cH,Angle360(palpha),-Angle360(palpha)+180);
                  path.lineTo(cX,cY+cH/2);

                  path.moveTo(p.x(),p.y());
                  path.arcTo(cX,cY,cW,cH,palpha-pdegree,360-Angle360(palpha-pdegree));
                  path.lineTo(cX+cW,cY+cH/2+pW);
                  path.arcTo(cX,cY+pW,cW,cH,0,-360+Angle360(palpha-pdegree));
              }

          }
          // 3)
          else if((q == 3 || q == 4) && (q_ == 1 || q_ == 2) && a>a_ )
          {
              path.moveTo(cX,cY+cH/2);
              path.arcTo(cX,cY,cW,cH,180,Angle360(palpha)-180);
              path.lineTo(p.x(),p.y()+pW);
              path.arcTo(cX,cY+pW,cW,cH,Angle360(palpha),-Angle360(palpha)+180);
              path.lineTo(cX,cY+cH/2);
          }
          // 4)
          else if((q == 1 || q == 2) && (q_ == 3 || q_ == 4) && a<a_)
          {
              p = GetPoint(palpha-pdegree);
              path.moveTo(p.x(),p.y());
              path.arcTo(cX,cY,cW,cH,palpha-pdegree,360-Angle360(palpha-pdegree));
              path.lineTo(cX+cW,cY+cH/2+pW);
              path.arcTo(cX,cY+pW,cW,cH,0,-360+Angle360(palpha-pdegree));
          }
          // 5)
          else if((q ==1 || q==2) && (q_==1 || q_==2) && a<a_)
          {
              path.moveTo(cX,cY+cH/2);
              path.arcTo(cX,cY,cW,cH,180,180);
              path.lineTo(cX+cW,cY+cH/2+pW);
              path.arcTo(cX,cY+pW,cW,cH,0,-180);
              path.lineTo(cX,cY+cH/2);
          }
          if (!path.isEmpty())
          {
              gradient_side.setColorAt(1,pieces[i].rgbColor);
              painter->setBrush(gradient_side);
              painter->drawPath(path);
          }
        }
    }
    else if (this->ctype==Nightcharts::Histogramm)
    {
        double pDist = 15;
        double pW = (cW-(pieces.size())*pDist)/pieces.size();

        QLinearGradient gradient(cX+cW/2,cY,cX+cW/2,cY+cH);
        gradient.setColorAt(0,Qt::black);
        QPen pen;
        pen.setWidth(3);

        for (int i=0;i<pieces.size();i++)
        {
            if (shadows)
            {
                painter->setPen(Qt::NoPen);
                painter->setBrush(Qt::darkGray);
                painter->drawRect(cX+pDist+i*(pW + pDist)-pDist/2,cY+cH-1,pW,-cH/100*pieces[i].pPerc+pDist/2-5);
            }
            gradient.setColorAt(1,pieces[i].rgbColor);
            painter->setBrush(gradient);
            pen.setColor(pieces[i].rgbColor);
            painter->setPen(pen);
            painter->drawRect(cX+pDist+i*(pW + pDist),cY+cH,pW,-cH/100*pieces[i].pPerc-5);
            QString label = QString::number(pieces[i].pPerc)+"%";
            painter->setPen(Qt::SolidLine);
            painter->drawText(cX+pDist+i*(pW + pDist)+pW/2-painter->fontMetrics().width(label)/2,cY+cH-cH/100*pieces[i].pPerc-painter->fontMetrics().height()/2,label);
        }
        painter->setPen(Qt::SolidLine);
        for (int i=1;i<10;i++)
        {
            painter->drawLine(cX-3,cY+cH/10*i,cX+3,cY+cH/10*i);    //妫殨 𐬠ﲨ Y
            //painter->drawText(cX-20,cY+cH/10*i,QString::number((10-i)*10)+"%");
        }
        painter->drawLine(cX,cY+cH,cX,cY);         //ﲼ Y
        painter->drawLine(cX,cY,cX+4,cY+10);       //򳱥쫨
        painter->drawLine(cX,cY,cX-4,cY+10);
        painter->drawLine(cX,cY+cH,cX+cW,cY+cH);   //ﲼ Ӎ

    }
    return 0;
}

int Nightcharts::drawLegend(QPainter *painter)
{
    //double ptext = 25;
    double angle = palpha;
    painter->setPen(Qt::SolidLine);

    switch(cltype)
    {
    /*case Nightcharts::Horizontal:
    {
        int dist = 5;
        painter->setBrush(Qt::white);
        float x = cX;
        float y = cY+cH+20+dist;
        //painter->drawRoundRect(cX+cW+20,cY,dist*2+200,pieces.size()*(fontmetr.height()+2*dist)+dist,15,15);
        for (int i=0;i<pieces.size();i++)
        {
            painter->setBrush(pieces[i].rgbColor);
            x += fontmetr.height()+2*dist;
            if (i%3 == 0)
            {
                x = cX;
                y += dist+fontmetr.height();
            }
            painter->drawRect(x,y,fontmetr.height(),fontmetr.height());
            QString label = pieces[i].pname + " - " + QString::number(pieces[i].pPerc)+"%";
            painter->drawText(x+fontmetr.height()+dist,y+fontmetr.height()/2+dist,label);
            x += fontmetr.width(label);
        }
        break;
    }*/
    case Nightcharts::Vertical:
    {
        int dist = 5;
        painter->setBrush(Qt::white);
        //painter->drawRoundRect(cX+cW+20,cY,dist*2+200,pieces.size()*(painter->fontMetrics().height()+2*dist)+dist,15,15);
        for (int i=pieces.size()-1;i>=0;i--)
        {
            painter->setBrush(pieces[i].rgbColor);
            float x = lX+dist;
            float y = lY+dist+i*(painter->fontMetrics().height()+2*dist);
            painter->drawRect(x,y,painter->fontMetrics().height(),painter->fontMetrics().height());
            painter->drawText(x+painter->fontMetrics().height()+dist,y+painter->fontMetrics().height()/2+dist,pieces[i].pname + "\t" + QString::number(pieces[i].pPerc,'g', 3)+"%");
        }
        break;
    }
    case Nightcharts::Round:
        for (int i=pieces.size()-1;i>=0;i--)
        {
            float len = 100;
            double pdegree = 3.6*pieces[i].pPerc;
            angle -= pdegree/2;
            QPointF p = GetPoint(angle);
            QPointF p_ = GetPoint(angle, cW+len,cH+len);
            int q = GetQuater(angle);
            if (q == 3 || q == 4)
            {
                p.setY(p.y()+pW/2);
                p_.setY(p_.y()+pW/2);
            }
            painter->drawLine(p.x(),p.y(),p_.x(),p_.y());
            QString label = pieces[i].pname + " - " + QString::number(pieces[i].pPerc)+"%";
            float recW = painter->fontMetrics().width(label)+10;
            float recH = painter->fontMetrics().height()+10;
            p_.setX(p_.x()-recW/2 + recW/2*cos(angle*M_PI/180));
            p_.setY(p_.y()+recH/2 + recH/2*sin(angle*M_PI/180));
            painter->setBrush(Qt::white);
            painter->drawRoundRect(p_.x() ,p_.y(), recW, -recH);
            painter->drawText(p_.x()+5, p_.y()-recH/2+5, label);
            angle -= pdegree/2;
         }
        break;
    }
    
    return 0;
}

QPointF Nightcharts::GetPoint(double angle, double R1, double R2)
{
    if (R1 == 0 && R2 == 0)
    {
        R1 = cW;
        R2 = cH;
    }
    QPointF point;
    double x = R1/2*cos(angle*M_PI/180);
    x+=cW/2+cX;
    double y = -R2/2*sin(angle*M_PI/180);
    y+=cH/2+cY;
    point.setX(x);
    point.setY(y);
    return point;
}

int Nightcharts::GetQuater(double angle)
{
    return ((int)Angle360(angle) / 90) + 1;
}

double Nightcharts::Angle360(double angle)
{
    int i = (int)angle;
    double delta = angle - i;
    return (i % 360 + delta);
}

pieceNC::pieceNC()
{
}
void pieceNC::addName(QString name)
{
    pname = name;
}
void pieceNC::setColor(Qt::GlobalColor color)
{
    rgbColor = color;
}
void pieceNC::setColor(QColor color)
{
    rgbColor = color;
}

void pieceNC::setPerc(float Percentage)
{
    pPerc = Percentage;
}
