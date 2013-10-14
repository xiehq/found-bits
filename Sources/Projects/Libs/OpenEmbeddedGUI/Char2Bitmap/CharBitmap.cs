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

namespace Char2Bitmap
{
  class CharBitmap
  {
    public CharBitmap(int Width, int Height)
    {
      this.bitmap = new bool[Width, Height];
      for (int x = 0; x < Width; x++)
      {
        for (int y = 0; y < Height; y++)
        {
          this.bitmap[x, y] = false;
        }
      }
    }
    private char ch;
    public char GetChar() { return ch; }
    public void SetChar(char ch) { this.ch = ch; }
    private bool[,] bitmap; // monochrome bitmap char
    public void SetForegroundColor(int x, int y)
    {
      this.bitmap[x, y] = true;
    }
    public void SetBackgroundColor(int x, int y)
    {
      this.bitmap[x, y] = false;
    }
    public bool IsForegroundColor(int x, int y)
    {
      return this.bitmap[x, y];
    }
    public int GetWidth()
    {
      return this.bitmap.GetLength(0);
    }
    public int GetHeight()
    {
      return this.bitmap.GetLength(1);
    }

    public string GetSource(Padding padding)
    {
      StringBuilder sb = new StringBuilder();
      int pixCount = 0;
      byte currentByte = 0;
      for (int Y = padding.Top; Y < GetHeight() - padding.Bottom; Y++)
      {
        for (int X = padding.Left; X < GetWidth() - padding.Right; X++)
        {
          if (IsForegroundColor(X, Y))
            currentByte |= 0x80;
          pixCount++;
          if (0 == (pixCount % 8))
          {
            sb.AppendFormat(", 0x{0:X2}", currentByte);
            currentByte = 0;
          }
          else
          {
            currentByte = (byte)(currentByte >> 1);
          }
        }
      }
      int remainingBits = pixCount % 8;
      if (0 != remainingBits)
      {
        currentByte >>= 7 - remainingBits;
        sb.AppendFormat(", 0x{0:X2}", currentByte);
      }

      return sb.ToString();
    }
    
    public string GetSource()
    {
      return GetSource(new Padding());
    }

    public int GetNumBytes()
    {
      int numPix = GetWidth() * GetHeight();
      return (numPix + 7) / 8;
    }

    public class Padding
    {
      public int Top = 0;
      public int Bottom = 0;
      public int Left = 0;
      public int Right = 0;
      public override string ToString() { return string.Format("Top:{0:d} Bottom:{1:d} Left:{2:d} Right:{3:d}", Top, Bottom, Left, Right); }
    }


    enum PaddingSide
    {
      Top,
      Bottom,
      Left,
      Right
    }

    private int getPading(PaddingSide paddingSide)
    {
      bool directionVerticalNotHorizontal = true;
      if (PaddingSide.Left == paddingSide || PaddingSide.Right == paddingSide)
        directionVerticalNotHorizontal = false;

      int dim1From = 0;
      int dim1To = 0;
      int dim2To = 0;
      int dim1Step = 0;

      switch (paddingSide)
      {
        case PaddingSide.Top: dim1From = 0; dim1To = GetHeight() - 1; dim2To = GetWidth() - 1; dim1Step = 1; break;
        case PaddingSide.Bottom: dim1From = GetHeight() - 1; dim1To = 0; dim2To = GetWidth() - 1; dim1Step = -1; break;
        case PaddingSide.Left: dim1From = 0; dim1To = GetWidth() - 1; dim2To = GetHeight() - 1; dim1Step = 1; break;
        case PaddingSide.Right: dim1From = GetWidth() - 1; dim1To = 0; dim2To = GetHeight() - 1; dim1Step = -1; break;
      }

      int pos1 = dim1From;

      for (int p1 = 0; p1 < Math.Abs(dim1From - dim1To) + 1; p1++)
      {
        bool brejk = false;

        int pos2 = 0;
        while (pos2 <= dim2To)
        {
          if (directionVerticalNotHorizontal)
          {
            if (IsForegroundColor(pos2, pos1))
            {
              brejk = true;
              break;
            }
          }
          else
          {
            if (IsForegroundColor(pos1, pos2))
            {
              brejk = true;
              break;
            }
          }
          pos2++;
        }

        if (brejk)
          break;
        pos1 = pos1 + dim1Step;
      }

      if (1 == dim1Step)
      {
        return pos1;
      }
      return dim1From - pos1;
    }
    public Padding GetPadding()
    {
      Padding padding = new Padding();

      padding.Top = getPading(PaddingSide.Top);
      padding.Bottom = getPading(PaddingSide.Bottom);
      padding.Left = getPading(PaddingSide.Left);
      padding.Right = getPading(PaddingSide.Right);

      if (padding.Top == padding.Bottom && padding.Top == GetHeight())
        padding.Bottom = 0;

      if (padding.Left == padding.Right && padding.Left == GetWidth())
        padding.Right = 0;

      return padding;
    }
  }

  class EmbeddedFont
  {
    public EmbeddedFont(int Height)
    {
      this.height = Height;
    }
    private int height;
    private List<CharBitmap> chars = new List<CharBitmap>();
    public List<CharBitmap> Chars { get { return this.chars; } }
    public CharBitmap NewChar(int Width)
    {
      CharBitmap cb = new CharBitmap(Width, height);
      chars.Add(cb);
      return cb;
    }

  }
}
