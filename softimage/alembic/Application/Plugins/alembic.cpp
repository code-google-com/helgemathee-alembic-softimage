// alembicPlugin
// Initial code generated by Softimage SDK Wizard
// Executed Fri Aug 19 09:14:49 UTC+0200 2011 by helge
// 
// Tip: You need to compile the generated code before you can load the plug-in.
// After you compile the plug-in, you can load it by clicking Update All in the Plugin Manager.
#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_menu.h>
#include <xsi_uitoolkit.h>
#include <xsi_comapihandler.h>
#include <xsi_project.h>
#include <xsi_selection.h>

using namespace XSI; 

#include "AlembicWriteJob.h"

SICALLBACK XSILoadPlugin( PluginRegistrar& in_reg )
{
	in_reg.PutAuthor(L"helge");
	in_reg.PutName(L"alembicPlugin");
	in_reg.PutVersion(1,0);
	in_reg.RegisterCommand(L"alembic_export",L"alembic_export");
	in_reg.RegisterMenu(siMenuMainFileExportID,L"alembic_Menu",false,false);
	//RegistrationInsertionPoint - do not remove this line

	return CStatus::OK;
}

SICALLBACK XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
	CString strPluginName;
	strPluginName = in_reg.GetName();
	Application().LogMessage(strPluginName + L" has been unloaded.",siVerboseMsg);
	return CStatus::OK;
}

SICALLBACK alembic_export_Init( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	Command oCmd;
	oCmd = ctxt.GetSource();
	oCmd.PutDescription(L"");
	oCmd.EnableReturnValue(true);

	ArgumentArray oArgs;
	oArgs = oCmd.GetArguments();
	oArgs.Add(L"filename");
	oArgs.AddWithHandler(L"objects",L"Collection");
	oArgs.Add(L"frameIn",1.0);
	oArgs.Add(L"frameOut",100.0);
	oArgs.Add(L"frameStep",1.0);
	return CStatus::OK;
}

SICALLBACK alembic_export_Execute( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	CValueArray args = ctxt.GetAttribute(L"Arguments");
	
   // take care of the filename
   CString filename = (CString)args[0].GetAsText();
   if(filename.IsEmpty())
   {
      // let's see if we are in interactive mode
      if(Application().IsInteractive())
      {
         UIToolkit toolkit = Application().GetUIToolkit();
         CComAPIHandler cToolkit(toolkit.GetRef());
         CComAPIHandler cBrowser(cToolkit.GetProperty(L"FileBrowser"));
         cBrowser.PutProperty(L"InitialDirectory",Application().GetActiveProject().GetPath());
         cBrowser.PutProperty(L"Filter",L"Alembic Files(*.abc)|*.abc|");
         CValue returnVal;
         cBrowser.Invoke(L"ShowSave",CComAPIHandler::Method,returnVal);
         filename = cBrowser.GetProperty(L"FilePathName").GetAsText();
         if(filename.IsEmpty())
            return CStatus::Abort;
      }
      else
      {
         Application().LogMessage(L"[alembic] No filename specified.",siErrorMsg);
         return CStatus::InvalidArgument;
      }
   }
   Application().LogMessage(L"[alembic] filename used: "+filename);

   // check the objects
	CRefArray objects = (CRefArray)args[1];
   if(objects.GetCount() == 0)
   {
      // use the selection
      objects = Application().GetSelection().GetArray();
      if(objects.GetCount() == 0)
      {
         Application().LogMessage(L"[alembic] No objects specified.",siErrorMsg);
         return CStatus::InvalidArgument;
      }
   }
   Application().LogMessage(L"[alembic] objects used: "+CValue(objects.GetCount()).GetAsText());

   // check the frames
   double frameIn = (double)args[2];
   double frameOut = (double)args[3];
   double frameStep = (double)args[4];
   CDoubleArray frames;
   for(double frame=frameIn; frame<=frameOut; frame+=frameStep)
      frames.Add(frame);
   Application().LogMessage(L"[alembic] frames used: "+CValue(frameIn).GetAsText()+L" to "+CValue(frameOut).GetAsText()+L", step "+CValue(frameStep).GetAsText());

   // run the job
   AlembicWriteJob job(filename,objects,frames);
	return job.Process();
}

SICALLBACK alembic_Menu_Init( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	Menu oMenu;
	oMenu = ctxt.GetSource();
	MenuItem oNewItem;
	oMenu.AddCommandItem(L"Alembic 1.0",L"alembic_export",oNewItem);
	return CStatus::OK;
}

