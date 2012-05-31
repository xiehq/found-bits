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

﻿namespace Char2Bitmap
{
  partial class Form1
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
      this.toolStrip1 = new System.Windows.Forms.ToolStrip();
      this.toolStripButtonChooseFont = new System.Windows.Forms.ToolStripButton();
      this.toolStripButtonGenerateSources = new System.Windows.Forms.ToolStripButton();
      this.pictureBoxPreviewAll = new System.Windows.Forms.PictureBox();
      this.listBoxFontChars = new System.Windows.Forms.ListBox();
      this.toolStrip1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBoxPreviewAll)).BeginInit();
      this.SuspendLayout();
      // 
      // toolStrip1
      // 
      this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButtonChooseFont,
            this.toolStripButtonGenerateSources});
      this.toolStrip1.Location = new System.Drawing.Point(0, 0);
      this.toolStrip1.Name = "toolStrip1";
      this.toolStrip1.Size = new System.Drawing.Size(404, 25);
      this.toolStrip1.TabIndex = 0;
      this.toolStrip1.Text = "toolStrip1";
      // 
      // toolStripButtonChooseFont
      // 
      this.toolStripButtonChooseFont.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButtonChooseFont.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonChooseFont.Image")));
      this.toolStripButtonChooseFont.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButtonChooseFont.Name = "toolStripButtonChooseFont";
      this.toolStripButtonChooseFont.Size = new System.Drawing.Size(23, 22);
      this.toolStripButtonChooseFont.Text = "ChooseFont";
      this.toolStripButtonChooseFont.ToolTipText = "Choose Font";
      this.toolStripButtonChooseFont.Click += new System.EventHandler(this.toolStripButtonChooseFont_Click);
      // 
      // toolStripButtonGenerateSources
      // 
      this.toolStripButtonGenerateSources.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButtonGenerateSources.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonGenerateSources.Image")));
      this.toolStripButtonGenerateSources.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButtonGenerateSources.Name = "toolStripButtonGenerateSources";
      this.toolStripButtonGenerateSources.Size = new System.Drawing.Size(23, 22);
      this.toolStripButtonGenerateSources.Text = "GenerateSources";
      this.toolStripButtonGenerateSources.ToolTipText = "Generate Sources";
      // 
      // pictureBoxPreviewAll
      // 
      this.pictureBoxPreviewAll.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.pictureBoxPreviewAll.Location = new System.Drawing.Point(113, 28);
      this.pictureBoxPreviewAll.Name = "pictureBoxPreviewAll";
      this.pictureBoxPreviewAll.Size = new System.Drawing.Size(279, 301);
      this.pictureBoxPreviewAll.TabIndex = 1;
      this.pictureBoxPreviewAll.TabStop = false;
      // 
      // listBoxFontChars
      // 
      this.listBoxFontChars.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
      this.listBoxFontChars.FormattingEnabled = true;
      this.listBoxFontChars.IntegralHeight = false;
      this.listBoxFontChars.Location = new System.Drawing.Point(12, 28);
      this.listBoxFontChars.Name = "listBoxFontChars";
      this.listBoxFontChars.Size = new System.Drawing.Size(95, 301);
      this.listBoxFontChars.TabIndex = 2;
      this.listBoxFontChars.SelectedIndexChanged += new System.EventHandler(this.listBoxFontChars_SelectedIndexChanged);
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(404, 335);
      this.Controls.Add(this.listBoxFontChars);
      this.Controls.Add(this.pictureBoxPreviewAll);
      this.Controls.Add(this.toolStrip1);
      this.Name = "Form1";
      this.Text = "Form1";
      this.toolStrip1.ResumeLayout(false);
      this.toolStrip1.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBoxPreviewAll)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.ToolStrip toolStrip1;
    private System.Windows.Forms.ToolStripButton toolStripButtonChooseFont;
    private System.Windows.Forms.ToolStripButton toolStripButtonGenerateSources;
    private System.Windows.Forms.PictureBox pictureBoxPreviewAll;
    private System.Windows.Forms.ListBox listBoxFontChars;
  }
}

