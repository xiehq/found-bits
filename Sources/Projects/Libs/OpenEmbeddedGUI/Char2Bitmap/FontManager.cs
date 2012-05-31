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
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;

namespace Char2Bitmap
{
  class FontManager
  {
    private Font font = null;
    public Font GetFont() { return font; }
    public bool CooseFontAndCreateBitmaps()
    {
      FontDialog fd = new FontDialog();
      fd.AllowVectorFonts = false;
      fd.AllowVerticalFonts = false;
      fd.ShowEffects = false;
      if (DialogResult.OK != fd.ShowDialog())
      {
        return false;
      }

      this.font = fd.Font;
      createBitmaps();
      return true;
    }

    private EmbeddedFont embeddedFont;
    public EmbeddedFont GetEmbeddedFont() { return embeddedFont; }
    private void createBitmaps()
    {
      int height = this.font.Height;
      this.embeddedFont = new EmbeddedFont(height);
      for (char c = ' '; c < 0x7F; c++)
      {
        Color[,] bm = createBitmap(c);
        CharBitmap cbm = embeddedFont.NewChar(bm.GetLength(0));
        cbm.SetChar(c);
        for (int x = 0; x < cbm.GetWidth(); x++)
        {
          for (int y = 0; y < cbm.GetHeight(); y++)
          {
            if (bm[x, y].ToArgb() == Color.Black.ToArgb())
            {
              cbm.SetForegroundColor(x, y);
            }
            else
            {
              cbm.SetBackgroundColor(x, y);
            }
          }
        }
      }
    }

    private Color[,] createBitmap(char c)
    {
      Bitmap bm = new Bitmap(1, 1);
      Graphics gr = Graphics.FromImage(bm);
      gr.TextRenderingHint = System.Drawing.Text.TextRenderingHint.SingleBitPerPixel;
      SizeF s = gr.MeasureString(c.ToString(), this.font);

      bm = new Bitmap((int)s.Width, (int)s.Height);
      gr = Graphics.FromImage(bm);
      gr.DrawString(c.ToString(), this.font, new SolidBrush(Color.Black), 0, 0, new StringFormat());

      Color[,] aColor = new Color[bm.Width, bm.Height];
      int pix = 0;
      for (int x = 0; x < bm.Width; x++)
      {
        for (int y = 0; y < bm.Height; y++)
        {
          Color color = bm.GetPixel(x, y);
          aColor[x, y] = color;
          pix++;
        }
      }

      return aColor;
    }
  }
}
