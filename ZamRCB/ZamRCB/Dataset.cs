using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using IEC61850.Common;
using IEC61850.Client;


namespace ZamRCB
{
    class Dataset
    {
        public string iedref = Form1.iedref;
        public IedConnection con = Form1.con;
        public DataSet ds = null;
        public string nom = null;
        public string dsref = null;
        Dictionary<string, string> dicval = Form1.dicval;
        Dictionary<string, string> dicfc = Form1.dicfc;
        Dictionary<string, string> dictyp = Form1.dictyp;

        public void Lire()
       {
            try
            {
                dicval.Clear();
                dicfc.Clear();
                dictyp.Clear();
                dsref = Form1.dsref; 
                List<string> fcdas = con.GetDataSetDirectory(dsref);
                for (int f = 0; f < fcdas.Count; f++)
                {
                    string fcdaref = fcdas[f];
                    string[] str = fcdaref.Split(new char[] { '[' });
                    nom = str[0];
                    string fc = str[1].Substring(0, 2);
                    string type = "";
                    string valeur = "";
                    //fcdaref = fcdaref.Substring(0, fcdaref.Length - 4);
                    MmsVariableSpecification vspec = con.GetVariableSpecification(nom, getFC(fc));
                    MmsType mmstype = vspec.GetType();
                    switch (mmstype.ToString())
                    {
                        case "MMS_BOOLEAN":
                            type = "BOOLEAN";
                            valeur = con.ReadBooleanValue(nom, getFC(fc)).ToString();
                            break;
                        case "MMS_INTEGER":
                            type = "INTEGER";
                            valeur = con.ReadIntegerValue(nom, getFC(fc)).ToString();
                            break;
                        case "MMS_UTC_TIME":
                            type = "TIMESTAMP";
                            Timestamp ts = con.ReadTimestampValue(nom, getFC(fc));
                            valeur = ts.GetTimeInMilliseconds().ToString();
                            break;
                        case "MMS_VISIBLE_STRING":
                            type = "STRING";
                            valeur = con.ReadStringValue(nom, getFC(fc)).ToString();
                            break;
                        case "MMS_FLOAT":
                            type = "FLOAT";
                            valeur = con.ReadFloatValue(nom, getFC(fc)).ToString();
                            break;
                        default:
                            Console.WriteLine(mmstype.ToString() + " inconnu");
                            break;
                    }
                    dicval[nom] = valeur;
                    dicfc[nom] = fc;
                    dictyp[nom] = type;
                }   // for FCDAS
            }   // try
            catch (IedConnectionException e)
            {
                string erreur = e.GetErrorCode().ToString();
            }
       } // Lire
       public FunctionalConstraint getFC(string fc)
       {
            FunctionalConstraint FC = 0;
            switch (fc)
            {
                case "SP":
                    FC = FunctionalConstraint.SP;
                    break;
                case "ST":
                    FC = FunctionalConstraint.ST;
                    break;
                case "MX":
                    FC = FunctionalConstraint.MX;
                    break;
                case "DC":
                    FC = FunctionalConstraint.DC;
                    break;
                case "CF":
                    FC = FunctionalConstraint.CF;
                    break;
                default:
                    FC = 0;
                    break;
            }
            return FC;
       } // getfc

       /// <summary>
       /// recupère la valeur d'une variable 61850
       /// adresse sous la forme [ adresse6850,fc]
       /// </summary>
       public string getValeur(List<string> adresse)
       {
            string res = "";
            string ad = adresse[0];
            string fc = adresse[1];
            string type = adresse[2];
            //MessageBox.Show(" recuperation de valeur avec l'adresse: " + ad + "," + fc + "," + type);
            // ici recuperationn des donnee sivant le fc et le type de da
            FunctionalConstraint FC = 0; // la unctional constraint
            FC = getFC(fc);
            try
            {
                IedConnection con = new IedConnection();
                string prov = Form1.IPserveur;
                con.Connect(prov, 102);
                switch (type)
                {
                    case "QUALITY":
                        Quality mmsquality = con.ReadQualityValue(ad, getFC(fc));
                        res = mmsquality.ToString();
                        break;

                    case "TIMESTAMP":
                        MmsValue mmsvalue = con.ReadValue(ad, getFC(fc));
                        ulong temps = mmsvalue.GetUtcTimeInMs();
                        res = temps.ToString();
                        break;

                    case "BOOLEAN":
                        res = con.ReadBooleanValue(ad, getFC(fc)).ToString();
                        break;

                    case "INTEGER":
                        res = con.ReadIntegerValue(ad, getFC(fc)).ToString();
                        break;

                    case "INT32":
                        MmsValue m3svalue = con.ReadValue(ad, getFC(fc));
                        res = m3svalue.ToString();
                        break;

                    case "FLOAT":
                        float f = con.ReadFloatValue(ad, getFC(fc));
                        res = con.ReadFloatValue(ad, getFC(fc)).ToString();
                        // res = con.ReadFloatValue(ad, FunctionalConstraint.SP).ToString();

                        break;
                    case "STRING":
                        res = con.ReadStringValue(ad, getFC(fc)).ToString();
                        break;

                    default:

                        break;
                }
                //con.Close();
                //con.Dispose();
            }

            catch (Exception ee)
            {
                throw new Exception("ER: getValeur " + ad + " " + ee.Message);
            }
            return res;
            //float ff = con.ReadFloatValue("IEDEVLDEV1/FSCH1.ValASG1.setMag.f", FunctionalConstraint.SP);
       } // getvaleur

    }   // Dataset
}   // namespace
