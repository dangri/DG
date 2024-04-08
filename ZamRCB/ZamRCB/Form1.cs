using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using IEC61850.Client;
using IEC61850.Common;


namespace ZamRCB
{
    public partial class Form1 : Form
    {
        public static string IPserveur;
        public static string rcbref;
        public static string dsref;
        public static string iedref;
        public static IedConnection con = new IedConnection();
        public static Dictionary<string, string> dicval = new Dictionary<string, string>();
        public static Dictionary<string, string> dicfc = new Dictionary<string, string>();
        public static Dictionary<string, string> dictyp = new Dictionary<string, string>();
        public static string newref;
        public static string newval;
        public static string exval;
        public static IntPtr hlv;
        public ReportControlBlock rcb;
        delegate void button1_ClickCallback(object sender, EventArgs e);
        public static Mutex mut = new Mutex();
 
        public Form1()
        {
            InitializeComponent();
        }

         public void btn_ON_Click(object sender, EventArgs e)
        {
            if (!rcb.GetRptEna())
            {
                rcb.SetRptEna(true);
                rcb.SetRCBValues();
            }

        }

        public void btnLec_Click(object sender, EventArgs e)
        {
            listView1.Items.Clear();
            listView1.Refresh();
            listView1.Show();
            con.Close();
            con.Connect(IPserveur, 102);
            iedref = rcbref.Substring(0, 5);
            Dataset ds = new Dataset();
            rcb = con.GetReportControlBlock(rcbref);
            rcb.GetRCBValues();
            dsref = rcb.GetDataSetReference();
            //mut.WaitOne();
            ds.Lire();
            //mut.ReleaseMutex();
            foreach (KeyValuePair<string, string> var in dicval)
            {
                ListViewItem itemn = new ListViewItem(var.Key);
                listView1.Items.Add(itemn);
                itemn.SubItems.Add(var.Value);
            }
            List<string> datarefs = con.GetDataSetDirectory(dsref);
            rcb.InstallReportHandler(reportH, datarefs);
            rcb.SetRptEna(true);
            //rcb.SetRCBValues(false);
            listView1.Refresh();
            listView1.Show();
        } //btnlec_click

        private void btn_OFF_Click(object sender, EventArgs e)
        {
            if (rcb.GetRptEna())
            {
                rcb.SetRptEna(false);
                rcb.SetRCBValues();
            }
        }

        public void reportH(Report report, object parameter)
        {
            List<string> datarefs = (List<string>)parameter;
            MmsValue values = report.GetDataSetValues();
            for (int i = 0; i < values.Size(); i++)
            {
                if (report.GetReasonForInclusion(i) != ReasonForInclusion.REASON_NOT_INCLUDED)
                {
                    string val = values.GetElement(i).ToString();
                    string nom = datarefs.ElementAt(i);
                    string[] str = nom.Split(new char[] { '[' });
                    nom = str[0];
                    newref = nom;
                    newval = val;
                    ClickB(null, EventArgs.Empty );
                }
            }
        }

        private void ClickB(object sender, EventArgs e)
        {
            if (this.listView1.InvokeRequired)
            {
                button1_ClickCallback btnc = new button1_ClickCallback(ClickB);
                this.Invoke(btnc, new object[] { sender, e });
            }
            else
            {
                this.button1_Click(null, EventArgs.Empty);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(newref == null) return;
            if (newref.Length > 0) 
            {
                ListViewItem item = listView1.FindItemWithText(newref);
                if (item != null)
                {
                    item.SubItems[1].Text = newval;
                    listView1.Refresh();
                    listView1.Show();
                }
            }
        }

        private void listView1_DoubleClick(object sender, EventArgs e)
        {
            ListViewItem item = listView1.SelectedItems[0];
            string reference = item.Text;
            newref = reference;
            string sfc = dicfc[reference];
            if (sfc == "SP")
            {
                Form form = new Form2();
                form.Text = newref;
                form.ShowDialog();
            }
        }

        //private void tbAdIP_KeyPress(object sender, KeyPressEventArgs e)
        //{
        //    if (e.KeyChar == 13)
        //    {
        //        con.Close();
        //        con.Dispose();
        //        con = new IedConnection();
        //        IPserveur = tbAdIP.Text;
        //        listView1.Items.Clear();
        //        listView1.Refresh();
        //        listView1.Show();
        //        comboBox1.Select();
        //        //Thread.Sleep(500);
        //    }
        //}

        private void comboBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 13)
            {
                IPserveur = comboBox1.Text.Substring(0, 13);
                rcbref = comboBox1.Text.Substring(14);
                btnLec_Click(null, EventArgs.Empty);
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            con.Close();
            if (rcb != null)
            {
                rcb.Dispose();
            }
            string[] s = comboBox1.Text.Split('-');
            IPserveur = s[0];
            rcbref = s[1];
            btnLec_Click(null, EventArgs.Empty);
        }

        private void listView1_ItemActivate(object sender, EventArgs e)
        {
            ListViewItem item = listView1.SelectedItems[0];
            string reference = item.Text;
            newref = reference;
            string sfc = dicfc[reference];
            if (sfc == "SP")
            {
                Form form = new Form2();
                form.Text = newref;
                form.ShowDialog();
            }
            btnLec_Click(null, e);
            this.Refresh();
            this.Show();
        }
     }   //Form1
}   //namespace
