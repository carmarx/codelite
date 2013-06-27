//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "settersgettersmodel.h"
#include <wx/dvrenderers.h>
#include <wx/variant.h>

//////////////////////////////////////////
// Implementation
//////////////////////////////////////////

// -------------------------------------------------
// Help method
// -------------------------------------------------
wxVariant SettersGettersModel::CreateIconTextVariant(const wxString &text, const wxBitmap& bmp)
{
    wxIcon icn;
    icn.CopyFromBitmap( bmp);
    wxDataViewIconText ict(text, icn);
    wxVariant v;
    v << ict;
    return v;
}

// -------------------------------------------------
// The model class
// -------------------------------------------------

SettersGettersModel::SettersGettersModel()
    : m_colCount(0)
{
}

SettersGettersModel::~SettersGettersModel()
{
    for(size_t i=0; i<m_data.size(); ++i) {
        delete m_data.at(i);
    }
    m_data.clear();
}

unsigned int SettersGettersModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const
{
    if(item.GetID() == NULL) {
        // Root
        for(size_t i=0; i<m_data.size(); ++i) {
            children.Add( wxDataViewItem( m_data.at(i) ) );
        }
        return children.size();
    }

    children.Clear();
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.m_pItem);
    if ( node ) {
        for(size_t i=0; i<node->GetChildren().size(); ++i) {
            children.Add( wxDataViewItem( node->GetChildren().at(i) ) );
        }
    }
    return children.GetCount();
}

unsigned int SettersGettersModel::GetColumnCount() const
{
    return m_colCount;
}

wxString SettersGettersModel::GetColumnType(unsigned int col) const
{
    if ( !m_data.empty() && m_data.at(0)->GetData().size() > col ) {
        return m_data.at(0)->GetData().at(col).GetType();
    }
    return "string";
}

wxDataViewItem SettersGettersModel::GetParent(const wxDataViewItem& item) const
{
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.m_pItem);
    if ( node ) {
        return wxDataViewItem(node->GetParent());
    }
    return wxDataViewItem(NULL);
}

bool SettersGettersModel::IsContainer(const wxDataViewItem& item) const
{
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.m_pItem);
    if ( node ) {
        return node->IsContainer();
    }
    return false;
}

void SettersGettersModel::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const
{
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.m_pItem);
    if ( node && node->GetData().size() > col ) {
        variant = node->GetData().at(col);
    }
}
wxDataViewItem SettersGettersModel::DoAppendItem(const wxDataViewItem& parent, const wxVector<wxVariant>& data, bool isContainer, wxClientData *clientData)
{
    SettersGettersModel_Item* parentNode = reinterpret_cast<SettersGettersModel_Item*>(parent.m_pItem);
    DoChangeItemType(parent, true);
    
    SettersGettersModel_Item* child = new SettersGettersModel_Item();
    child->SetIsContainer(isContainer);
    child->SetClientObject( clientData );
    child->SetData( data );
    if ( parentNode ) {
        parentNode->AddChild( child );

    } else {
        m_data.push_back( child );
    }

    return wxDataViewItem(child);
}

wxDataViewItem SettersGettersModel::DoInsertItem(const wxDataViewItem& insertBeforeMe, const wxVector<wxVariant>& data, bool isContainer, wxClientData *clientData)
{
    SettersGettersModel_Item* child = new SettersGettersModel_Item();
    child->SetIsContainer(isContainer);
    child->SetClientObject( clientData );
    child->SetData( data );

    // find the location where to insert the new item
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(insertBeforeMe.m_pItem);
    if ( !node )
        return wxDataViewItem();

    wxVector<SettersGettersModel_Item*>::iterator where = std::find(m_data.begin(), m_data.end(), node);

    if ( where !=  m_data.end() ) {
        // top level item
        m_data.insert( where, child );

    } else {

        if ( !node->GetParent() )
            return wxDataViewItem();

        child->SetParent(node->GetParent());
        where = std::find(node->GetParent()->GetChildren().begin(), node->GetParent()->GetChildren().end(), node);
        if ( where == node->GetParent()->GetChildren().end() ) {
            node->GetParent()->GetChildren().push_back( child );

        } else {
            node->GetParent()->GetChildren().insert(where, child);

        }
    }

    return wxDataViewItem(child);
}

wxDataViewItem SettersGettersModel::AppendItem(const wxDataViewItem &parent, const wxVector<wxVariant>& data, wxClientData *clientData)
{
    wxDataViewItem ch = DoAppendItem(parent, data, false, clientData);
    ItemAdded(parent, ch);
    return ch;
}

wxDataViewItemArray SettersGettersModel::AppendItems(const wxDataViewItem &parent, const wxVector<wxVector<wxVariant> >& data)
{
    wxDataViewItemArray items;
    for(size_t i=0; i<data.size(); ++i) {
        items.push_back( DoAppendItem(parent, data.at(i), false, NULL) );
    }
    ItemsAdded(parent, items);
    return items;
}

bool SettersGettersModel::SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col)
{
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.m_pItem);
    if ( node && node->GetData().size() > col ) {
        node->GetData().at(col) = variant;
    }
    return true;
}

void SettersGettersModel::DeleteItem(const wxDataViewItem& item)
{
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.m_pItem);
    if ( node ) {
        
        SettersGettersModel_Item* parent = node->GetParent();
        wxDataViewItem parentItem(parent);
        ItemDeleted(parentItem, item);
        
        // this will also remove it from its model parent children list
        if ( parent == NULL ) {
            // root item, remove it from the roots array
            wxVector<SettersGettersModel_Item*>::iterator where = std::find(m_data.begin(), m_data.end(), node);
            if ( where != m_data.end() ) {
                m_data.erase(where);
            }
        }
        
        // If there are no more children, change the item back to 'normal'
        if ( parent && parent->GetChildren().empty() )
            DoChangeItemType(parentItem, false);
            
        delete node;
    }
    
    if ( IsEmpty() )
        Cleared();
}

void SettersGettersModel::DeleteItems(const wxDataViewItem& parent, const wxDataViewItemArray& items)
{
    // sanity
    for(size_t i=0; i<items.GetCount(); ++i) {
        SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(items.Item(i).m_pItem);
        wxUnusedVar(node);
        wxASSERT(node && node->GetParent() == parent.m_pItem);
        DeleteItem(items.Item(i));
    }
}

void SettersGettersModel::Clear()
{
    wxVector<SettersGettersModel_Item*> roots = m_data;
    wxVector<SettersGettersModel_Item*>::iterator iter = roots.begin();
    for(; iter != roots.end(); ++iter) {
        DeleteItem( wxDataViewItem(*iter) );
    }
    Cleared();
}

bool SettersGettersModel::IsEmpty() const
{
    return m_data.empty();
}

wxClientData* SettersGettersModel::GetClientObject(const wxDataViewItem& item) const
{
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.GetID());
    if ( node ) {
        return node->GetClientObject();
    }
    return NULL;
}

void SettersGettersModel::SetClientObject(const wxDataViewItem& item, wxClientData *data)
{
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.GetID());
    if ( node ) {
        node->SetClientObject(data);
    }
}

void SettersGettersModel::UpdateItem(const wxDataViewItem& item, const wxVector<wxVariant>& data)
{
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.GetID());
    if ( node ) {
        node->SetData( data );
        ItemChanged( item );
    }
}

wxDataViewItem SettersGettersModel::InsertItem(const wxDataViewItem& insertBeforeMe, const wxVector<wxVariant>& data, wxClientData *clientData)
{
    wxDataViewItem ch = DoInsertItem(insertBeforeMe, data, false, clientData);
    if ( ch.IsOk() ) {
        SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(ch.GetID());
        ItemAdded(wxDataViewItem(node->GetParent()), ch);
    }
    return ch;
}

wxVector<wxVariant> SettersGettersModel::GetItemColumnsData(const wxDataViewItem& item) const
{
    if ( !item.IsOk() )
        return wxVector<wxVariant>();

    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.GetID());
    if ( !node ) {
        return wxVector<wxVariant>();
    }
    return node->GetData();
}

bool SettersGettersModel::HasChildren(const wxDataViewItem& item) const
{
    if ( !item.IsOk() )
        return false;

    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.GetID());
    if ( !node ) {
        return false;
    }
    return !node->GetChildren().empty();
}

void SettersGettersModel::DoChangeItemType(const wxDataViewItem& item, bool changeToContainer)
{
    SettersGettersModel_Item* node = reinterpret_cast<SettersGettersModel_Item*>(item.GetID());
    if ( !node )
        return;
    
    if ( ( changeToContainer && !node->IsContainer())  || // change an item from non-container to container type
         ( !changeToContainer && node->IsContainer()) ) { // change an item from container to non-container type
#if defined(__WXGTK__) || defined(__WXMAC__)
        // change the item to container type:
        // 1st we need to delete it
        ItemDeleted(wxDataViewItem(node->GetParent()), item);
        
        // update the node type
        node->SetIsContainer(changeToContainer);
        ItemAdded(wxDataViewItem(node->GetParent()), item);
#else
        node->SetIsContainer(changeToContainer);
#endif
    }
}
