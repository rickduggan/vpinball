#include "StdAfx.h"
#include "resource.h"
#include "ImageDialog.h"
#include "vpversion.h"

typedef struct _tagSORTDATA
{
    HWND hwndList;
    int subItemIndex;
    int sortUpDown;
}SORTDATA;

extern SORTDATA SortData;
extern int CALLBACK MyCompProc( LPARAM lSortParam1, LPARAM lSortParam2, LPARAM lSortOption );
extern int CALLBACK MyCompProcIntValues(LPARAM lSortParam1, LPARAM lSortParam2, LPARAM lSortOption);
int ImageDialog::m_columnSortOrder;
bool ImageDialog::m_doNotChange;

ImageDialog::ImageDialog() : CDialog(IDD_IMAGEDIALOG)
{
   m_columnSortOrder = 1;
   m_doNotChange = false;
}

ImageDialog::~ImageDialog()
{

}

void ImageDialog::OnDestroy()
{
   CDialog::OnDestroy();
}

void ImageDialog::OnClose()
{
   SavePosition();
   CDialog::OnClose();
}

BOOL ImageDialog::OnInitDialog()
{   
   m_columnSortOrder = 1;
   return TRUE;
}

INT_PTR ImageDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   HWND hwndDlg = GetHwnd();
   CCO(PinTable) * const pt = g_pvp->GetActiveTable();

   m_resizer.HandleMessage(uMsg, wParam, lParam);

   switch (uMsg)
   {
      case WM_INITDIALOG:
      {
         LVCOLUMN lvcol;
         HWND hListView = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
         m_resizer.Initialize(*this, CRect(0, 0, 500, 600));
         m_resizer.AddChild(hListView, topleft, RD_STRETCH_WIDTH | RD_STRETCH_HEIGHT);
         m_resizer.AddChild(GetDlgItem(IDC_PICTUREPREVIEW).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_IMPORT).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_REIMPORT).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_REIMPORTFROM).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_UPDATE_ALL_BUTTON).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_DELETE_IMAGE).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_RENAME).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_EXPORT).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_OK).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_ALPHA_MASK_EDIT).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_STATIC_ALPHA).GetHwnd(), topright, 0);
         m_resizer.AddChild(GetDlgItem(IDC_CHECK_RENAME_ON_EXPORT).GetHwnd(), topright, 0);

         LoadPosition();

         ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

         lvcol.mask = LVCF_TEXT | LVCF_WIDTH;
         LocalString ls(IDS_NAME);
         lvcol.pszText = ls.m_szbuffer;// = "Name";
         lvcol.cx = 100;
         ListView_InsertColumn(hListView, 0, &lvcol);

         LocalString ls2(IDS_IMPORTPATH);
         lvcol.pszText = ls2.m_szbuffer; // = "Import Path";
         lvcol.cx = 200;
         ListView_InsertColumn(hListView, 1, &lvcol);

         LocalString ls3(IDS_IMAGESIZE);
         lvcol.pszText = ls3.m_szbuffer; // = "Image Size";
         lvcol.cx = 100;
         ListView_InsertColumn(hListView, 2, &lvcol);

         LocalString ls4( IDS_USED_IN_TABLE );
         lvcol.pszText = ls4.m_szbuffer; // = "In use";
         lvcol.cx = 45;
         ListView_InsertColumn( hListView, 3, &lvcol );

         LocalString ls5(IDS_IMAGE_RAW_SIZE);
         lvcol.pszText = ls5.m_szbuffer; // = "Raw Size";
         lvcol.cx = 60;
         ListView_InsertColumn(hListView, 4, &lvcol);

         if (pt)
            pt->ListImages(hListView);

         char textBuf[16];
         strcpy_s(textBuf, "128");
         SetDlgItemText(IDC_ALPHA_MASK_EDIT, textBuf);
         ListView_SetItemState(hListView, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
         GotoDlgCtrl(hListView);
         return FALSE;
      }
      case GET_COLOR_TABLE:
      {
         if ( pt )
            *((unsigned long **)lParam) = pt->m_rgcolorcustom;
         return TRUE;
      }
      case WM_NOTIFY:
      {
         LPNMHDR pnmhdr = (LPNMHDR)lParam;
         if (wParam == IDC_SOUNDLIST)
         {
            LPNMLISTVIEW lpnmListView = (LPNMLISTVIEW)lParam;
            if (lpnmListView->hdr.code == LVN_COLUMNCLICK)
            {
               const int columnNumber = lpnmListView->iSubItem;
               if (m_columnSortOrder == 1)
                  m_columnSortOrder = 0;
               else
                  m_columnSortOrder = 1;
               SortData.hwndList = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
               SortData.subItemIndex = columnNumber;
               SortData.sortUpDown = m_columnSortOrder;
               if (columnNumber==4)
                   ListView_SortItems(SortData.hwndList, MyCompProcIntValues, &SortData);
               else
                   ListView_SortItems(SortData.hwndList, MyCompProc, &SortData);
            }
         }

         switch (pnmhdr->code)
         {
            case LVN_ENDLABELEDIT:
            {
               NMLVDISPINFO * const pinfo = (NMLVDISPINFO *)lParam;
               const HWND hSoundlist = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
               if (pinfo->item.pszText == NULL || pinfo->item.pszText[0] == '\0')
               {
                  return FALSE;
               }
               ListView_SetItemText(hSoundlist, pinfo->item.iItem, 0, pinfo->item.pszText);
               LVITEM lvitem;
               lvitem.mask = LVIF_PARAM;
               lvitem.iItem = pinfo->item.iItem;
               lvitem.iSubItem = 0;
               ListView_GetItem(hSoundlist, &lvitem);
               Texture * const ppi = (Texture *)lvitem.lParam;
               if (ppi != NULL)
               {
                  strncpy_s(ppi->m_szName, pinfo->item.pszText, MAXTOKEN);
                  strncpy_s(ppi->m_szInternalName, pinfo->item.pszText, MAXTOKEN);
                  CharLowerBuff(ppi->m_szInternalName, lstrlen(ppi->m_szInternalName));
                  if (pt)
                     pt->SetNonUndoableDirty(eSaveDirty);
               }
               return TRUE;
         }
         break;

         case LVN_ITEMCHANGING:
         {
            if (m_doNotChange)
               break;
            NMLISTVIEW * const plistview = (LPNMLISTVIEW)lParam;
            if ((plistview->uNewState & LVIS_SELECTED) != (plistview->uOldState & LVIS_SELECTED))
            {
               if (plistview->uNewState & LVIS_SELECTED)
               {
                  const int sel = plistview->iItem;
                  LVITEM lvitem;
                  lvitem.mask = LVIF_PARAM;
                  lvitem.iItem = sel;
                  lvitem.iSubItem = 0;
                  ListView_GetItem(GetDlgItem(IDC_SOUNDLIST).GetHwnd(), &lvitem);
                  Texture * const ppi = (Texture *)lvitem.lParam;
                  if (ppi != NULL)
                  {
                     char textBuf[256];
                     sprintf_s(textBuf, "%i", (int)ppi->m_alphaTestValue);
                     SetDlgItemText(IDC_ALPHA_MASK_EDIT, textBuf);
                  }
               }
               ::InvalidateRect(GetDlgItem(IDC_PICTUREPREVIEW).GetHwnd(), NULL, fTrue);
            }
         }
         break;

         case LVN_ITEMCHANGED:
         {
            if (m_doNotChange)
               break;
            NMLISTVIEW * const plistview = (LPNMLISTVIEW)lParam;
            const int sel = plistview->iItem;
            LVITEM lvitem;
            lvitem.mask = LVIF_PARAM;
            lvitem.iItem = sel;
            lvitem.iSubItem = 0;
            ListView_GetItem(GetDlgItem(IDC_SOUNDLIST).GetHwnd(), &lvitem);
            Texture * const ppi = (Texture *)lvitem.lParam;
            if (ppi != NULL)
            {
               const CString textStr = GetDlgItemText(IDC_ALPHA_MASK_EDIT);
               const float v = sz2f(textStr.c_str());
               if (ppi->m_alphaTestValue != v)
               {
                  ppi->m_alphaTestValue = v;
                  pt->SetNonUndoableDirty(eSaveDirty);
               }

               const int count = ListView_GetSelectedCount(GetDlgItem(IDC_SOUNDLIST).GetHwnd());
               const int fEnable = !(count > 1);
               ::EnableWindow(GetDlgItem(IDC_REIMPORTFROM).GetHwnd(), fEnable);
               ::EnableWindow(GetDlgItem(IDC_RENAME).GetHwnd(), fEnable);
            }
            //EnableWindow(GetDlgItem(hwndDlg, IDC_EXPORT), fEnable);
         }
         break;

         }
      }
      break;

      case WM_DRAWITEM:
      {
         const HWND hSoundList = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
         DRAWITEMSTRUCT * const pdis = (DRAWITEMSTRUCT *)lParam;
         const int sel = ListView_GetNextItem(hSoundList, -1, LVNI_SELECTED);
         if (sel != -1)
         {
            LVITEM lvitem;
            lvitem.mask = LVIF_PARAM;
            lvitem.iItem = sel;
            lvitem.iSubItem = 0;
            ListView_GetItem(hSoundList, &lvitem);
            Texture * const ppi = (Texture *)lvitem.lParam;
            if (ppi != NULL)
            {
               RECT rcClient;
               ::GetWindowRect(pdis->hwndItem, &rcClient);

               const int xsize = rcClient.right - rcClient.left;
               const int ysize = rcClient.bottom - rcClient.top;

               const float controlaspect = (float)xsize / (float)ysize;
               const float aspect = (float)ppi->m_width / (float)ppi->m_height;

               int width, height;
               if (aspect > controlaspect)
               {
                  width = xsize;
                  height = (int)(xsize / aspect);
               }
               else
               {
                  height = ysize;
                  width = (int)(ysize*aspect);
               }

               const int x = (xsize - width) / 2;
               const int y = (ysize - height) / 2;

               HDC hdcDD;
               ppi->GetTextureDC(&hdcDD);
               SetStretchBltMode(pdis->hDC, HALFTONE); // somehow enables filtering
               StretchBlt(pdis->hDC, x, y, width, height, hdcDD, 0, 0, ppi->m_width, ppi->m_height, SRCCOPY);
               ppi->ReleaseTextureDC(hdcDD);
            }
         }
         else
         {
            // Nothing currently selected
            RECT rcClient;
            ::GetClientRect(pdis->hwndItem, &rcClient);
            SelectObject(pdis->hDC, GetStockObject(NULL_BRUSH));
            SelectObject(pdis->hDC, GetStockObject(BLACK_PEN));
            Rectangle(pdis->hDC, rcClient.left + 5, rcClient.top + 5, rcClient.right - 5, rcClient.bottom - 5);

            SetBkMode(pdis->hDC, TRANSPARENT);

            RECT rcText = rcClient;

            //ExtTextOut(pdis->hDC, 0, 20, 0, NULL, "Image\nPreview", 13, NULL);
            LocalString ls(IDS_IMAGE_PREVIEW);
            const int len = lstrlen(ls.m_szbuffer);
            DrawText(pdis->hDC, ls.m_szbuffer/*"Image\n\nPreview"*/, len, &rcText, DT_CALCRECT);

            const int halfheight = (rcClient.bottom - rcClient.top) / 2;
            const int halffont = (rcText.bottom - rcText.top) / 2;

            rcText.left = rcClient.left;
            rcText.right = rcClient.right;
            rcText.top = halfheight - halffont;
            rcText.bottom = halfheight + halffont;

            DrawText(pdis->hDC, ls.m_szbuffer/*"Image\n\nPreview"*/, len, &rcText, DT_CENTER);
         }
         return TRUE;
      }
      break;

   }
   return DialogProcDefault(uMsg, wParam, lParam);
}

void ImageDialog::UpdateImages()
{
    HWND hSoundList = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
    CCO(PinTable) * const pt = g_pvp->GetActiveTable();

    const int count = ListView_GetSelectedCount(hSoundList);
    if (count > 0)
    {
        int sel = ListView_GetNextItem(hSoundList, -1, LVNI_SELECTED);
        while (sel != -1)
        {
            LVITEM lvitem;
            lvitem.mask = LVIF_PARAM;
            lvitem.iItem = sel;
            lvitem.iSubItem = 0;
            ListView_GetItem(hSoundList, &lvitem);
            Texture * const ppi = (Texture *)lvitem.lParam;
            if (ppi != NULL)
            {
                const CString textStr = GetDlgItemText(IDC_ALPHA_MASK_EDIT);
                const float v = sz2f(textStr.c_str());
                if (ppi->m_alphaTestValue != v)
                {
                    ppi->m_alphaTestValue = v;
                    pt->SetNonUndoableDirty(eSaveDirty);
                }

                sel = ListView_GetNextItem(hSoundList, sel, LVNI_SELECTED);
            }
        }
        SetFocus();
    }

}

BOOL ImageDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
   UNREFERENCED_PARAMETER(lParam);
   HWND hSoundList = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
   CCO(PinTable) * const pt = g_pvp->GetActiveTable();

   switch (LOWORD(wParam))
   {
      case IDC_IMPORT: Import(); break;
      case IDC_EXPORT: Export(); break;
      case IDC_DELETE_IMAGE: DeleteImage(); break;
      case IDC_REIMPORT: Reimport(); break;
      case IDC_UPDATE_ALL_BUTTON: UpdateAll(); break;
      case IDC_REIMPORTFROM: ReimportFrom(); break;
      case IDC_RENAME:
      {
         const int sel = ListView_GetNextItem(hSoundList, -1, LVNI_SELECTED);
         if (sel != -1)
         {
            ::SetFocus(hSoundList);
            ListView_EditLabel(hSoundList, sel);
            pt->SetNonUndoableDirty(eSaveDirty);
            g_pvp->m_sb.PopulateDropdowns();
            g_pvp->m_sb.RefreshProperties();
         }
         break;
      }
      case IDC_OK:
      {
         const int count = ListView_GetSelectedCount(hSoundList);
         if (count > 0)
         {
            UpdateImages();
            SetFocus();
         }
         g_pvp->m_sb.PopulateDropdowns();
         g_pvp->m_sb.RefreshProperties();

         SavePosition();
         CDialog::OnOK();
         break;
      }
   }
   return FALSE;
}

void ImageDialog::OnOK()
{
   // do not call CDialog::OnOk() here because if you rename images keys like backspace or escape in rename mode cause an IDOK message and this function is called
}

void ImageDialog::OnCancel()
{
    SavePosition();
    CDialog::OnCancel();
}

void ImageDialog::Import()
{
   const HWND hSoundList = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
   CCO(PinTable) * const pt = g_pvp->GetActiveTable();
   char szFileName[MAXSTRING];
   char szInitialDir[MAXSTRING];
   szFileName[0] = '\0';

   OPENFILENAME ofn;
   ZeroMemory(&ofn, sizeof(OPENFILENAME));
   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hInstance = g_hinst;
   ofn.hwndOwner = g_pvp->m_hwnd;

   ofn.lpstrFilter = "Bitmap, JPEG, PNG, TGA, EXR, HDR Files (.bmp/.jpg/.png/.tga/.exr/.hdr)\0*.bmp;*.jpg;*.jpeg;*.png;*.tga;*.exr;*.hdr\0";
   ofn.lpstrFile = szFileName;
   ofn.nMaxFile = MAXSTRING;
   ofn.lpstrDefExt = "png";
   ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ALLOWMULTISELECT;

   HRESULT hr = LoadValueString("RecentDir", "ImageDir", szInitialDir, MAXSTRING);
   ofn.lpstrInitialDir = (hr == S_OK) ? szInitialDir : NULL;

   const int ret = GetOpenFileName(&ofn);

   if (ret)
   {
      strcpy_s(szInitialDir, sizeof(szInitialDir), szFileName);

      int len = lstrlen(szFileName);
      if (len < ofn.nFileOffset)
      {
         // Multi-file select
         char szT[MAXSTRING];
         lstrcpy(szT, szFileName);
         lstrcat(szT, "\\");
         len++;
         int filenamestart = ofn.nFileOffset;
         int filenamelen = lstrlen(&szFileName[filenamestart]);
         while (filenamelen > 0)
         {
            lstrcpy(&szT[len], &szFileName[filenamestart]);
            pt->ImportImage(hSoundList, szT);
            filenamestart += filenamelen + 1;
            filenamelen = lstrlen(&szFileName[filenamestart]);
         }
      }
      else
      {
         szInitialDir[ofn.nFileOffset] = 0;
         pt->ImportImage(hSoundList, szFileName);
      }
      hr = SaveValueString("RecentDir", "ImageDir", szInitialDir);
      pt->SetNonUndoableDirty(eSaveDirty);
      SetFocus();
      g_pvp->m_sb.PopulateDropdowns();
      g_pvp->m_sb.RefreshProperties();
   }
}

void ImageDialog::Export()
{
   const HWND hSoundList = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
   CCO(PinTable) * const pt = g_pvp->GetActiveTable();
   char g_filename[MAX_PATH];
   char g_initDir[MAX_PATH];
   const int selectedItemsCount = ListView_GetSelectedCount(hSoundList);
      
   const size_t renameOnExport = SendMessage(GetDlgItem(IDC_CHECK_RENAME_ON_EXPORT).GetHwnd(), BM_GETCHECK, 0, 0);

   if (selectedItemsCount)	// if some items are selected???
   {
      int sel = ListView_GetNextItem(hSoundList, -1, LVNI_SELECTED);
      if (sel != -1)
      {
         LVITEM lvitem;
         lvitem.mask = LVIF_PARAM;
         lvitem.iItem = sel;
         lvitem.iSubItem = 0;
         ListView_GetItem(hSoundList, &lvitem);
         Texture * ppi = (Texture*)lvitem.lParam;
         if (ppi != NULL)
         {
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hInstance = g_hinst;
            ofn.hwndOwner = g_pvp->m_hwnd;
            int len = lstrlen(ppi->m_szPath);
            memset(g_filename, 0, MAX_PATH);
            memset(g_initDir, 0, MAX_PATH);

            if (!renameOnExport)
            {
               int begin; //select only file name from pathfilename
               for (begin = len; begin >= 0; begin--)
               {
                  if (ppi->m_szPath[begin] == '\\')
                  {
                     begin++;
                     break;
                  }
               }
               if (begin > 0)
               {
                  memcpy(g_filename, &ppi->m_szPath[begin], len - begin);
                  g_filename[len - begin] = 0;
               }
            }
            else
            {
               strcat_s(g_filename, ppi->m_szName);
               const string ext(ppi->m_szPath);
               const size_t idx = ext.find_last_of(".");
               strcat_s(g_filename, ext.c_str() + idx);
            }
            ofn.lpstrFile = g_filename;
            ofn.nMaxFile = MAX_PATH;

            const string ext2(g_filename);
            const size_t idx2 = ext2.find_last_of(".");
            ofn.lpstrDefExt = ext2.c_str() + idx2 + 1;
            // check which default file extension should be selected
            ofn.lpstrFilter = "PNG (.png)\0*.png;\0Bitmap (.bmp)\0*.bmp;\0JPEG (.jpg/.jpeg)\0*.jpg;*.jpeg;\0IFF (.iff)\0*.IFF;\0PCX (.pcx)\0*.PCX;\0PICT (.pict)\0*.PICT;\0Photoshop (.psd)\0*.psd;\0TGA (.tga)\0*.tga;\0TIFF (.tiff/.tif)\0*.tiff;*.tif;\0EXR (.exr)\0*.exr;\0HDR (.hdr)\0*.hdr\0";
            if(!strcmp(ofn.lpstrDefExt,"png"))
               ofn.nFilterIndex = 1;
            else if (!strcmp(ofn.lpstrDefExt, "bmp"))
               ofn.nFilterIndex = 2;
            else if (!strcmp(ofn.lpstrDefExt, "jpg") || !strcmp(ofn.lpstrDefExt, "jpeg"))
               ofn.nFilterIndex = 3;
            else if (!strcmp(ofn.lpstrDefExt, "iff"))
               ofn.nFilterIndex = 4;
            else if (!strcmp(ofn.lpstrDefExt, "pcx"))
               ofn.nFilterIndex = 5;
            else if (!strcmp(ofn.lpstrDefExt, "pict"))
               ofn.nFilterIndex = 6;
            else if (!strcmp(ofn.lpstrDefExt, "psd"))
               ofn.nFilterIndex = 7;
            else if (!strcmp(ofn.lpstrDefExt, "tga"))
               ofn.nFilterIndex = 8;
            else if (!strcmp(ofn.lpstrDefExt, "tif") || !strcmp(ofn.lpstrDefExt, "tiff"))
               ofn.nFilterIndex = 9;
            else if (!strcmp(ofn.lpstrDefExt, "exr"))
               ofn.nFilterIndex = 10;
            else if (!strcmp(ofn.lpstrDefExt, "hdr"))
               ofn.nFilterIndex = 11;

            const HRESULT hr = LoadValueString("RecentDir", "ImageDir", g_initDir, MAX_PATH);

            ofn.lpstrInitialDir = (hr == S_OK) ? g_initDir : NULL;
            //ofn.lpstrTitle = "SAVE AS";
            ofn.Flags = OFN_NOREADONLYRETURN | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_EXPLORER;

            g_initDir[ofn.nFileOffset] = 0;

            if (GetSaveFileName(&ofn))	//Get filename from user
            {
               len = lstrlen(ofn.lpstrFile);
               int begin; //select only file name from pathfilename
               for (begin = len; begin >= 0; begin--)
               {
                  if (ofn.lpstrFile[begin] == '\\')
                  {
                     begin++;
                     break;
                  }
               }
               char pathName[MAX_PATH] = { 0 };
               if (begin > 0)
               {
                  memcpy(pathName, ofn.lpstrFile, begin);
                  pathName[begin] = 0;
               }
               while (sel != -1 && ppi != NULL)
               {
                  len = lstrlen(ppi->m_szPath);
                  for (begin = len; begin >= 0; begin--)
                  {
                     if (ppi->m_szPath[begin] == '\\')
                     {
                        begin++;
                        break;
                     }
                  }
                  if (selectedItemsCount>1)
                  {
                     memset(g_filename, 0, MAX_PATH);
                     strcpy_s(g_filename, MAX_PATH, pathName);
                     if (!renameOnExport)
                        strcat_s(g_filename, MAX_PATH, &ppi->m_szPath[begin]);
                     else
                     {
                        strcat_s(g_filename, ppi->m_szName);
                        const string ext(ppi->m_szPath);
                        const size_t idx = ext.find_last_of(".");
                        strcat_s(g_filename, ext.c_str() + idx);
                     }
                  }

                  if (!pt->ExportImage(ppi, g_filename)) //!! this will always export the image in its original format, no matter what was actually selected by the user
                     ShowError("Could not export Image");
                  sel = ListView_GetNextItem(hSoundList, sel, LVNI_SELECTED);
                  lvitem.iItem = sel;
                  lvitem.iSubItem = 0;
                  ListView_GetItem(hSoundList, &lvitem);
                  ppi = (Texture*)lvitem.lParam;
               }
               SaveValueString("RecentDir", "ImageDir", pathName);
            } // finished all selected items
         }
      }
   }

   SetFocus();
}

void ImageDialog::DeleteImage()
{
   const HWND hSoundList = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
   CCO(PinTable) * const pt = g_pvp->GetActiveTable();

   const int count = ListView_GetSelectedCount(hSoundList);
   if (count > 0)
   {
      LocalString ls(IDS_REMOVEIMAGE);
      const int ans = MessageBox( ls.m_szbuffer/*"Are you sure you want to remove this image?"*/, "Confirm Deletion", MB_YESNO | MB_DEFBUTTON2);
      if (ans == IDYES)
      {
         int sel = ListView_GetNextItem(hSoundList, -1, LVNI_SELECTED);
         int lastsel = -1;
         while (sel != -1)
         {
            LVITEM lvitem;
            lvitem.mask = LVIF_PARAM;
            lvitem.iItem = sel;
            lvitem.iSubItem = 0;
            ListView_GetItem(hSoundList, &lvitem);
            Texture * const ppi = (Texture*)lvitem.lParam;
            if (ppi != NULL)
            {
               m_doNotChange = true; // do not trigger LVN_ITEMCHANGING or LVN_ITEMCHANGED code!
               ListView_DeleteItem(hSoundList, sel);
               m_doNotChange = false;
               pt->RemoveImage(ppi);

               // The previous selection is now deleted, so look again from the top of the list
               lastsel = sel;
               sel = ListView_GetNextItem(hSoundList, -1, LVNI_SELECTED);
               g_pvp->m_sb.PopulateDropdowns();
               g_pvp->m_sb.RefreshProperties();
            }
         }
         if (lastsel != -1)
            ListView_SetItemState(hSoundList, (lastsel > 0) ? lastsel-1 : 0, LVNI_SELECTED | LVNI_FOCUSED, LVNI_SELECTED | LVNI_FOCUSED);
         pt->SetNonUndoableDirty(eSaveDirty);
      }
   }

   SetFocus();
}

void ImageDialog::Reimport()
{
   const HWND hSoundList = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
   CCO(PinTable) * const pt = g_pvp->GetActiveTable();

   const int count = ListView_GetSelectedCount(hSoundList);
   if (count > 0)
   {
      LocalString ls(IDS_REPLACEIMAGE);
      const int ans = MessageBox(ls.m_szbuffer/*"Are you sure you want to replace this image?"*/, "Confirm Reimport", MB_YESNO | MB_DEFBUTTON2);
      if (ans == IDYES)
      {
         int sel = ListView_GetNextItem(hSoundList, -1, LVNI_SELECTED);
         while (sel != -1)
         {
            LVITEM lvitem;
            lvitem.mask = LVIF_PARAM;
            lvitem.iItem = sel;
            lvitem.iSubItem = 0;
            ListView_GetItem(hSoundList, &lvitem);
            Texture * const ppi = (Texture*)lvitem.lParam;
            if (ppi != NULL)
            {
               HANDLE hFile = CreateFile(ppi->m_szPath, GENERIC_READ, FILE_SHARE_READ,
                  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

               if (hFile != INVALID_HANDLE_VALUE)
               {
                  CloseHandle(hFile);
                  pt->ReImportImage(ppi, ppi->m_szPath);
                  pt->SetNonUndoableDirty(eSaveDirty);
               }
               else MessageBox( ppi->m_szPath, "  FILE NOT FOUND!  ", MB_OK);
               sel = ListView_GetNextItem(hSoundList, sel, LVNI_SELECTED);
            }
         }
      }
      // Display new image
      ::InvalidateRect(GetDlgItem(IDC_PICTUREPREVIEW).GetHwnd(), NULL, fTrue);
   }
   SetFocus();
}

void ImageDialog::UpdateAll()
{
   const HWND hSoundList = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
   CCO(PinTable) * const pt = g_pvp->GetActiveTable();

   const int count = ListView_GetSelectedCount(hSoundList);
   bool errorOccurred = false;

   for (int sel = 0; sel < count; sel++)
   {
      LVITEM lvitem;
      lvitem.mask = LVIF_PARAM;
      lvitem.iItem = sel;
      lvitem.iSubItem = 0;
      ListView_GetItem(hSoundList, &lvitem);
      Texture * const ppi = (Texture*)lvitem.lParam;
      if (ppi != NULL)
      {
         HANDLE hFile = CreateFile(ppi->m_szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
         if (hFile != INVALID_HANDLE_VALUE)
         {
            CloseHandle(hFile);
            pt->ReImportImage(ppi, ppi->m_szPath);
            pt->SetNonUndoableDirty(eSaveDirty);
         }
         else
            errorOccurred = true;
      }
      // Display new image
      ::InvalidateRect(GetDlgItem(IDC_PICTUREPREVIEW).GetHwnd(), NULL, fTrue);
   }

   if (errorOccurred)
      MessageBox( "Not all images were updated!", "  FILES NOT FOUND!  ", MB_OK);
}

void ImageDialog::ReimportFrom()
{
   const HWND hSoundList = GetDlgItem(IDC_SOUNDLIST).GetHwnd();
   CCO(PinTable) * const pt = g_pvp->GetActiveTable();

   int sel = ListView_GetNextItem(hSoundList, -1, LVNI_SELECTED);
   if (sel != -1)
   {
      LocalString ls(IDS_REPLACEIMAGE);
      const int ans = MessageBox( ls.m_szbuffer/*"Are you sure you want to replace this image with a new one?"*/, "Confirm Reimport", MB_YESNO | MB_DEFBUTTON2);
      if (ans == IDYES)
      {
         char szFileName[MAXSTRING];
         szFileName[0] = '\0';

         OPENFILENAME ofn;
         ZeroMemory(&ofn, sizeof(OPENFILENAME));
         ofn.lStructSize = sizeof(OPENFILENAME);
         ofn.hInstance = g_hinst;
         ofn.hwndOwner = g_pvp->m_hwnd;
         // TEXT
         ofn.lpstrFilter = "Bitmap, JPEG, PNG, TGA, EXR, HDR Files (.bmp/.jpg/.png/.tga/.exr/.hdr)\0*.bmp;*.jpg;*.jpeg;*.png;*.tga;*.exr;*.hdr\0";
         ofn.lpstrFile = szFileName;
         ofn.nMaxFile = MAXSTRING;
         ofn.lpstrDefExt = "png";
         ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

         char szInitialDir[MAXSTRING];
         HRESULT hr = LoadValueString("RecentDir", "ImageDir", szInitialDir, MAXSTRING);
         ofn.lpstrInitialDir = (hr == S_OK) ? szInitialDir : NULL;

         const int ret = GetOpenFileName(&ofn);

         if (ret)
         {
            LVITEM lvitem;
            lvitem.mask = LVIF_PARAM;
            lvitem.iItem = sel;
            lvitem.iSubItem = 0;
            ListView_GetItem(hSoundList, &lvitem);
            Texture * const ppi = (Texture*)lvitem.lParam;
            if (ppi != NULL)
            {
               strcpy_s(szInitialDir, sizeof(szInitialDir), szFileName);
               szInitialDir[ofn.nFileOffset] = 0;
               hr = SaveValueString("RecentDir", "ImageDir", szInitialDir);

               pt->ReImportImage(ppi, ofn.lpstrFile);
               ListView_SetItemText(hSoundList, sel, 1, ppi->m_szPath);
               pt->SetNonUndoableDirty(eSaveDirty);

               // Display new image
               ::InvalidateRect(GetDlgItem(IDC_PICTUREPREVIEW).GetHwnd(), NULL, fTrue);
            }
         }
      }
   }

   SetFocus();
}

void ImageDialog::LoadPosition()
{
    const int x = LoadValueIntWithDefault( "Editor", "ImageMngPosX", 0 );
    const int y = LoadValueIntWithDefault( "Editor", "ImageMngPosY", 0 );

    const int w = LoadValueIntWithDefault("Editor", "ImageMngWidth", 1000);
    const int h = LoadValueIntWithDefault("Editor", "ImageMngHeight", 800);
    SetWindowPos(NULL, x, y, w, h, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE);
}

void ImageDialog::SavePosition()
{
    int w, h;
    CRect rect = GetWindowRect();

    SaveValueInt("Editor", "ImageMngPosX", rect.left);
    SaveValueInt("Editor", "ImageMngPosY", rect.top);
    w = rect.right - rect.left;
    SaveValueInt("Editor", "ImageMngWidth", w);
    h = rect.bottom - rect.top;
    SaveValueInt("Editor", "ImageMngHeight", h);
}
