/////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, pa.eeapai@gmail.com                                         //
// All rights reserved.                                                            //
//                                                                                 //
// Redistribution and use in source and binary forms, with or without              //
// modification, are permitted provided that the following conditions are met:     //
//     * Redistributions of source code must retain the above copyright            //
//       notice, this list of conditions and the following disclaimer.             //
//     * Redistributions in binary form must reproduce the above copyright         //
//       notice, this list of conditions and the following disclaimer in the       //
//       documentation and/or other materials provided with the distribution.      //
//     * Neither the name of the pa.eeapai@gmail.com nor the                       //
//       names of its contributors may be used to endorse or promote products      //
//       derived from this software without specific prior written permission.     //
//                                                                                 //
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND //
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   //
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          //
// DISCLAIMED. IN NO EVENT SHALL pa.eeapai@gmail.com BE LIABLE FOR ANY             //
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES      //
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;    //
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND     //
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT      //
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS   //
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    //
/////////////////////////////////////////////////////////////////////////////////////

﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Char2Bitmap
{
  public partial class Form1 : Form
  {
    private FontManager man = new FontManager();
    public Form1()
    {
      InitializeComponent();
    }

    private void toolStripButtonChooseFont_Click(object sender, EventArgs e)
    {
      if (!man.CooseFontAndCreateBitmaps())
        return;
      listBoxFontChars.Items.Clear();
      Graphics gr = pictureBoxPreviewAll.CreateGraphics();
      this.listBoxFontChars.Font = man.GetFont();
      foreach (CharBitmap cbm in man.GetEmbeddedFont().Chars)
      {
        listBoxFontChars.Items.Add(cbm.GetChar());
      }
    }

    private void listBoxFontChars_SelectedIndexChanged(object sender, EventArgs e)
    {
      Graphics gr = pictureBoxPreviewAll.CreateGraphics();
      gr.Clear(this.BackColor);
      int index = listBoxFontChars.SelectedIndex;
      if (index < 0 || index >= man.GetEmbeddedFont().Chars.Count)
        return;
      CharBitmap cbm = man.GetEmbeddedFont().Chars[index];

      int width = (int)gr.VisibleClipBounds.Width;
      int height = (int)gr.VisibleClipBounds.Height;

      int pixWidth = width / cbm.GetWidth();
      int pixHeight = height / cbm.GetHeight();

      int pixSize = Math.Min(pixWidth, pixHeight);

      int pixWidthOffset = (width - pixSize * cbm.GetWidth()) / 2;
      int pixHeightOffset = (height - pixSize * cbm.GetHeight()) / 2;

      int pixCount = 0;
      Color byteColor1 = Color.Green;
      Color byteColor2 = Color.Blue;
      Color byteColor = byteColor1;
      int gridWidth = 4;
      for (int Y = 0; Y < cbm.GetHeight(); Y++)
      {
        for (int X = 0; X < cbm.GetWidth(); X++)
        {
          Color pixColor = cbm.IsForegroundColor(X, Y) ? Color.Black : Color.White;
          SolidBrush brush = new SolidBrush(pixColor);
          gr.FillRectangle(brush, pixWidthOffset + X * pixSize, pixHeightOffset + Y * pixSize, pixSize - gridWidth, pixSize - gridWidth);
          Pen pen = new Pen(byteColor, gridWidth);
          gr.DrawRectangle(pen, pixWidthOffset + X * pixSize, pixHeightOffset + Y * pixSize, pixSize - gridWidth, pixSize - gridWidth);
          pen = new Pen(this.BackColor, 1);
          gr.DrawRectangle(pen, pixWidthOffset + X * pixSize - gridWidth/2, pixHeightOffset + Y * pixSize - gridWidth/2, pixSize, pixSize);
          pixCount++;

          if (0 == pixCount % 8)
          {
            if (byteColor1 == byteColor)
              byteColor = byteColor2;
            else
              byteColor = byteColor1;
          }
        }
      }
    }
  }
}
