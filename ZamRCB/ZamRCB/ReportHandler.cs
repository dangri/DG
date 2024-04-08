using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using IEC61850.Common;
using IEC61850.Client;


namespace DGClient
{
    class ReportHandler
    {
		private static void reportH(Report report, object parameter)
		{
			Console.WriteLine("Received report:\n----------------");

			if (report.HasTimestamp())
				Console.WriteLine("  timestamp: " + MmsValue.MsTimeToDateTimeOffset(report.GetTimestamp()).ToString());

			MmsValue values = report.GetDataSetValues();

			Console.WriteLine("  report dataset contains " + values.Size() + " elements");

			for (int i = 0; i < values.Size(); i++)
			{
				if (report.GetReasonForInclusion(i) != ReasonForInclusion.REASON_NOT_INCLUDED)
				{
					Console.WriteLine("    element " + i + " included for reason " + report.GetReasonForInclusion(i).ToString() + " " + values.GetElement(i));
				}
			}
		}

	}
}
