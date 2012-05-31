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
