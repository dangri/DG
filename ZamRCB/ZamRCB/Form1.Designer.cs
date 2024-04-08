namespace ZamRCB
{
    partial class Form1
    {
        /// <summary>
        /// Variable nécessaire au concepteur.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Nettoyage des ressources utilisées.
        /// </summary>
        /// <param name="disposing">true si les ressources managées doivent être supprimées ; sinon, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

         #region Code généré par le Concepteur Windows Form

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            this.lblnomRCB = new System.Windows.Forms.Label();
            this.btn_ON = new System.Windows.Forms.Button();
            this.btn_OFF = new System.Windows.Forms.Button();
            this.btnLec = new System.Windows.Forms.Button();
            this.listView1 = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.button1 = new System.Windows.Forms.Button();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // lblnomRCB
            // 
            this.lblnomRCB.AutoSize = true;
            this.lblnomRCB.Location = new System.Drawing.Point(26, 18);
            this.lblnomRCB.Name = "lblnomRCB";
            this.lblnomRCB.Size = new System.Drawing.Size(106, 13);
            this.lblnomRCB.TabIndex = 5;
            this.lblnomRCB.Text = "Référence du RCB : ";
            // 
            // btn_ON
            // 
            this.btn_ON.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.btn_ON.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.btn_ON.ForeColor = System.Drawing.Color.Green;
            this.btn_ON.Location = new System.Drawing.Point(189, 55);
            this.btn_ON.Name = "btn_ON";
            this.btn_ON.Size = new System.Drawing.Size(33, 20);
            this.btn_ON.TabIndex = 8;
            this.btn_ON.Text = "ON";
            this.btn_ON.UseVisualStyleBackColor = false;
            this.btn_ON.Click += new System.EventHandler(this.btn_ON_Click);
            // 
            // btn_OFF
            // 
            this.btn_OFF.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.btn_OFF.ForeColor = System.Drawing.Color.Red;
            this.btn_OFF.Location = new System.Drawing.Point(228, 55);
            this.btn_OFF.Name = "btn_OFF";
            this.btn_OFF.Size = new System.Drawing.Size(37, 20);
            this.btn_OFF.TabIndex = 9;
            this.btn_OFF.Text = "OFF";
            this.btn_OFF.UseVisualStyleBackColor = false;
            this.btn_OFF.Click += new System.EventHandler(this.btn_OFF_Click);
            // 
            // btnLec
            // 
            this.btnLec.Location = new System.Drawing.Point(130, 55);
            this.btnLec.Name = "btnLec";
            this.btnLec.Size = new System.Drawing.Size(53, 20);
            this.btnLec.TabIndex = 10;
            this.btnLec.Text = "Lecture";
            this.btnLec.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            this.btnLec.UseVisualStyleBackColor = true;
            this.btnLec.Click += new System.EventHandler(this.btnLec_Click);
            // 
            // listView1
            // 
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader3});
            this.listView1.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.listView1.HideSelection = false;
            this.listView1.Location = new System.Drawing.Point(29, 94);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(373, 547);
            this.listView1.TabIndex = 11;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            this.listView1.ItemActivate += new System.EventHandler(this.listView1_ItemActivate);
            this.listView1.DoubleClick += new System.EventHandler(this.comboBox1_SelectedIndexChanged);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Références";
            this.columnHeader1.Width = 244;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Valeur";
            this.columnHeader3.Width = 141;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(271, 55);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(22, 20);
            this.button1.TabIndex = 12;
            this.button1.Text = "R";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // comboBox1
            // 
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Items.AddRange(new object[] {
            "192.168.1.120-IEDCSDLDCS/LLN0.RP.urcbcss1",
            "192.168.1.120-IEDCSDLDCS/LLN0.RP.urcbcss2",
            "192.168.1.120-IEDCSDLDSE1/LLN0.RP.urcbev",
            "192.168.1.120-IEDCSDLDSE2/LLN0.RP.urcbev",
            "192.168.1.120-IEDCSDLDSE3/LLN0.RP.urcbev",
            "192.168.1.120-IEDCSDLDCS/LLN0.RP.urcbcs",
            "192.168.1.121-IEDSELDEVSE/LLN0.RP.urcbevse",
            "192.168.1.121-IEDSELDEV/LLN0.RP.urcbev",
            "192.168.1.121-IEDSELDEVSE/LLN0.RP.urcbs1c",
            "192.168.1.121-IEDSELDEVSE/LLN0.RP.urcbs1s1",
            "192.168.1.121-IEDSELDEVSE/LLN0.RP.urcbs1s2",
            "192.168.1.122-IEDSELDEVSE/LLN0.RP.urcbevse",
            "192.168.1.122-IEDSELDEV/LLN0.RP.urcbev",
            "192.168.1.122-IEDSELDEVSE/LLN0.RP.urcbs1c",
            "192.168.1.122-IEDSELDEVSE/LLN0.RP.urcbs1s1",
            "192.168.1.122-IEDSELDEVSE/LLN0.RP.urcbs1s2",
            "192.168.1.123-IEDSELDEVSE/LLN0.RP.urcbevse",
            "192.168.1.123-IEDSELDEV/LLN0.RP.urcbev",
            "192.168.1.123-IEDSELDEVSE/LLN0.RP.urcbs1c",
            "192.168.1.123-IEDSELDEVSE/LLN0.RP.urcbs1s1",
            "192.168.1.123-IEDSELDEVSE/LLN0.RP.urcbs1s2",
            "10.3.141.131-IEDEVLDEV1/LLN0.RP.urcbev",
            "10.3.141.131-IEDEVLDEV1/LLN0.RP.urcb01",
            "10.3.141.131-IEDEVLDEV1/LLN0.RP.urcbevc",
            "10.3.141.131-IEDEVLDEV1/LLN0.RP.urcbevs1",
            "10.3.141.131-IEDEVLDEV1/LLN0.RP.urcbevs2",
            "10.3.141.132-IEDEVLDEV1/LLN0.RP.urcbev",
            "10.3.141.132-IEDEVLDEV1/LLN0.RP.urcb01",
            "10.3.141.132-IEDEVLDEV1/LLN0.RP.urcbevc",
            "10.3.141.132-IEDEVLDEV1/LLN0.RP.urcbevs1",
            "10.3.141.132-IEDEVLDEV1/LLN0.RP.urcbevs2",
            "10.3.141.132-IEDEVLDEV1/LLN0.RP.urcbev",
            "10.3.141.133-IEDEVLDEV1/LLN0.RP.urcb01",
            "10.3.141.133-IEDEVLDEV1/LLN0.RP.urcbevc",
            "10.3.141.133-IEDEVLDEV1/LLN0.RP.urcbevs1",
            "10.3.141.133-IEDEVLDEV1/LLN0.RP.urcbevs2"});
            this.comboBox1.Location = new System.Drawing.Point(128, 15);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(274, 21);
            this.comboBox1.TabIndex = 13;
            this.comboBox1.SelectedIndexChanged += new System.EventHandler(this.comboBox1_SelectedIndexChanged);
            this.comboBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.comboBox1_KeyPress);
            // 
            // Form1
            // 
            this.AccessibleDescription = "public";
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(429, 653);
            this.Controls.Add(this.comboBox1);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.listView1);
            this.Controls.Add(this.btnLec);
            this.Controls.Add(this.btn_OFF);
            this.Controls.Add(this.btn_ON);
            this.Controls.Add(this.lblnomRCB);
            this.Name = "Form1";
            this.Text = "Report";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        public System.Windows.Forms.Label lblnomRCB;
        public System.Windows.Forms.Button btn_ON;
        public System.Windows.Forms.Button btn_OFF;
        public System.Windows.Forms.Button btnLec;
        private System.Windows.Forms.ListView listView1;
        public System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ComboBox comboBox1;
    }
}

