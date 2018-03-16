namespace LaserPrinter
{
    partial class Form4
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
            this.button1 = new System.Windows.Forms.Button();
            this.txtSendMssg = new System.Windows.Forms.TextBox();
            this.txtRecvMssg = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.txtRecvMssg2 = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.textport1 = new System.Windows.Forms.TextBox();
            this.textport2 = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.resvrport1 = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.resvrport2 = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(135, 31);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(59, 23);
            this.button1.TabIndex = 0;
            this.button1.Text = "发送1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // txtSendMssg
            // 
            this.txtSendMssg.Location = new System.Drawing.Point(13, 141);
            this.txtSendMssg.Multiline = true;
            this.txtSendMssg.Name = "txtSendMssg";
            this.txtSendMssg.Size = new System.Drawing.Size(491, 36);
            this.txtSendMssg.TabIndex = 1;
            // 
            // txtRecvMssg
            // 
            this.txtRecvMssg.Location = new System.Drawing.Point(16, 195);
            this.txtRecvMssg.Multiline = true;
            this.txtRecvMssg.Name = "txtRecvMssg";
            this.txtRecvMssg.Size = new System.Drawing.Size(491, 33);
            this.txtRecvMssg.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(14, 120);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 12);
            this.label1.TabIndex = 3;
            this.label1.Text = "发送数据：";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(14, 180);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(65, 12);
            this.label2.TabIndex = 4;
            this.label2.Text = "接收数据：";
            // 
            // txtRecvMssg2
            // 
            this.txtRecvMssg2.Location = new System.Drawing.Point(13, 246);
            this.txtRecvMssg2.Multiline = true;
            this.txtRecvMssg2.Name = "txtRecvMssg2";
            this.txtRecvMssg2.Size = new System.Drawing.Size(491, 41);
            this.txtRecvMssg2.TabIndex = 5;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(11, 231);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(71, 12);
            this.label3.TabIndex = 6;
            this.label3.Text = "接收数据2：";
            this.label3.Click += new System.EventHandler(this.label3_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(353, 34);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(63, 23);
            this.button2.TabIndex = 7;
            this.button2.Text = "发送2";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // textport1
            // 
            this.textport1.Location = new System.Drawing.Point(79, 33);
            this.textport1.Name = "textport1";
            this.textport1.Size = new System.Drawing.Size(50, 21);
            this.textport1.TabIndex = 10;
            this.textport1.Text = "2014";
            // 
            // textport2
            // 
            this.textport2.Location = new System.Drawing.Point(290, 36);
            this.textport2.Name = "textport2";
            this.textport2.Size = new System.Drawing.Size(57, 21);
            this.textport2.TabIndex = 11;
            this.textport2.Text = "2015";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(14, 36);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(59, 12);
            this.label4.TabIndex = 12;
            this.label4.Text = "发送端口:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(219, 39);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(65, 12);
            this.label5.TabIndex = 13;
            this.label5.Text = "发送端口2:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(14, 65);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(59, 12);
            this.label6.TabIndex = 15;
            this.label6.Text = "接收端口:";
            // 
            // resvrport1
            // 
            this.resvrport1.Location = new System.Drawing.Point(79, 62);
            this.resvrport1.Name = "resvrport1";
            this.resvrport1.Size = new System.Drawing.Size(50, 21);
            this.resvrport1.TabIndex = 14;
            this.resvrport1.Text = "2015";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(219, 66);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(65, 12);
            this.label7.TabIndex = 17;
            this.label7.Text = "接收端口2:";
            // 
            // resvrport2
            // 
            this.resvrport2.Location = new System.Drawing.Point(290, 63);
            this.resvrport2.Name = "resvrport2";
            this.resvrport2.Size = new System.Drawing.Size(57, 21);
            this.resvrport2.TabIndex = 16;
            this.resvrport2.Text = "2014";
            // 
            // Form4
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(516, 311);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.resvrport2);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.resvrport1);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textport2);
            this.Controls.Add(this.textport1);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txtRecvMssg2);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtRecvMssg);
            this.Controls.Add(this.txtSendMssg);
            this.Controls.Add(this.button1);
            this.Name = "Form4";
            this.Text = "Form4";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form4_FormClosing);
            this.Load += new System.EventHandler(this.Form4_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox txtSendMssg;
        private System.Windows.Forms.TextBox txtRecvMssg;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtRecvMssg2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.TextBox textport1;
        private System.Windows.Forms.TextBox textport2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox resvrport1;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox resvrport2;
    }
}