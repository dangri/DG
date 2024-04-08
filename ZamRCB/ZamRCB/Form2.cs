using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using IEC61850.Client;
using IEC61850.Common;



namespace ZamRCB
{
    public partial class Form2 : Form
    {
        public static string reference = null;
        public static string sval = null;

        public Form2()
        {
            InitializeComponent();
        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 13)
            {
                button1_Click(null, EventArgs.Empty);
                return;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            reference = Form1.newref;
            sval = textBox1.Text;  
            string sfc = Form1.dicfc[reference];
            string stype = Form1.dictyp[reference];
            FunctionalConstraint fc = 0;
            IedConnection con = Form1.con;
            switch (sfc)
            {
                case "SP":
                    fc = FunctionalConstraint.SP;
                    break;
                default:
                    break;
            }
            switch (stype)
            {
                case "BOOLEAN":
                    bool valb = Boolean.Parse(sval);
                    con.WriteValue(reference, fc, new MmsValue(valb));
                    break;
                case "INTEGER":
                    int vali = int.Parse(sval);
                    con.WriteValue(reference, fc, new MmsValue(vali));
                    break;
                case "FLOAT":
                    float valf = float.Parse(sval); //  float 
                    con.WriteValue(reference, fc, new MmsValue(valf));
                    break;
                //case "DOUBLE":
                //    double vald = double.Parse(sval);
                //    con.WriteValue(reference, fc, new MmsValue(vald));
                case "STRING":
                    string vals = sval;
                    con.WriteValue(reference, fc, new MmsValue(vals));
                    break;
                case "MMS_UTC_TIME":
                    UInt64 valt = UInt64.Parse(sval);
                    MmsValue mmsv = new MmsValue(valt);
                    break;
            }
            this.Dispose();
        }
    }
}

