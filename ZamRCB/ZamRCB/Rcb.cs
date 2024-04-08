using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using IEC61850.Client;
using IEC61850.Common;


namespace DGClient
{
    public class Rcb
    {
        public ReportControlBlock rcb;
        IedConnection con = Form1.con;
        string rcbref;
        string dsref;

        public void install()
        {
            try
            {
                rcbref = Form1.rcbref;
                rcb = con.GetReportControlBlock(rcbref); 
                rcb.GetRCBValues();
                dsref = rcb.GetDataSetReference();
                Form1.dsref = dsref;
                List<string> datarefs = con.GetDataSetDirectory(dsref);
                rcb.InstallReportHandler(reportH, datarefs);
                rcb.SetRptEna(false);
                rcb.SetRCBValues();
            }
            catch (IedConnectionException e)
            {
                string erreur = e.GetErrorCode().ToString();
            }
        } // install
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
                    //ListViewItem item = listView1.FindItemWithText(old);
                    //item.Text = nom + " -> " + val; // ( old, sref + " -> " + val);
                    //listView1.Refresh();
                    //ListView1.Show();
                }
            }
        }
    } // class
}   // namespace

