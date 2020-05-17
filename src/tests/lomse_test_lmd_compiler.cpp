//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Lomse is copyrighted work (c) 2010-2018. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#include <UnitTest++.h>
#include <sstream>
#include "lomse_build_options.h"

//classes related to these tests
#include "lomse_injectors.h"
#include "private/lomse_document_p.h"
#include "lomse_lmd_compiler.h"
#include "lomse_internal_model.h"

using namespace UnitTest;
using namespace std;
using namespace lomse;


class LmdCompilerTestFixture
{
public:

    LibraryScope m_libraryScope;
    std::string m_scores_path;

    LmdCompilerTestFixture()     //SetUp fixture
        : m_libraryScope(cout)
    {
        m_scores_path = TESTLIB_SCORES_PATH;
        m_libraryScope.set_default_fonts_path(TESTLIB_FONTS_PATH);
    }

    ~LmdCompilerTestFixture()    //TearDown fixture
    {
    }
};

SUITE(LmdCompilerTest)
{
    TEST_FIXTURE(LmdCompilerTestFixture, LmdCompilerCreateEmpty)
    {
        Document doc(m_libraryScope);
        LmdCompiler compiler(m_libraryScope, &doc);
        ImoObj* pRoot =  compiler.create_empty();
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>(pRoot);
        CHECK( pDoc->get_version() == "0.0" );
        CHECK( pDoc->get_num_content_items() == 0 );
        if (pRoot && !pRoot->is_document()) delete pRoot;
    }

    TEST_FIXTURE(LmdCompilerTestFixture, LmdCompilerFromString)
    {
        Document doc(m_libraryScope);
        LmdCompiler compiler(m_libraryScope, &doc);
        string src =
            "<lenmusdoc vers='0.0'>"
                "<content>"
                    "<ldpmusic>"
                        "(score (vers 1.6)(instrument (musicData (clef G)(n c4 q.))))"
                    "</ldpmusic>"
                "</content>"
            "</lenmusdoc>";
        ImoObj* pRoot =  compiler.compile_string(src);
        CHECK( compiler.get_file_locator() == "string:" );
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>(pRoot);
        CHECK( pDoc->get_version() == "0.0" );
        CHECK( pDoc->get_num_content_items() == 1 );
        ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
        CHECK( pScore != nullptr );
        CHECK( pScore->get_num_instruments() == 1 );
        CHECK( pScore->get_staffobjs_table() != nullptr );
        CHECK( pScore->get_version_string() == "1.6" );
        if (pRoot && !pRoot->is_document()) delete pRoot;
    }

    //TEST_FIXTURE(LmdCompilerTestFixture, LmdCompilerScoreIdsFixed)
    //{
    //    Document doc(m_libraryScope);
    //    LmdCompiler compiler(m_libraryScope, &doc);
    //    ImoObj* pRoot =  compiler.compile_string("(score (vers 1.6) (language en iso-8859-1) (systemLayout first (systemMargins 0 0 0 2000)) (systemLayout other (systemMargins 0 0 1200 2000)) (opt Score.FillPageWithEmptyStaves true) (opt StaffLines.Truncate 1) (instrument (musicData)))" );
    //    ImoDocument* pDoc = dynamic_cast<ImoDocument*>(pRoot);
    //    CHECK( pDoc->get_version() == "0.0" );
    //    CHECK( pDoc->get_num_content_items() == 1 );
    //    ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
    //    CHECK( pScore != nullptr );
    //    //cout << "id=" << pScore->get_id() << endl;
    //    CHECK( pScore->get_id() == 3L );
    //    CHECK( pScore->get_num_instruments() == 1 );
    //    ImoInstrument* pInstr = pScore->get_instrument(0);
    //    CHECK( pInstr->get_id() == 12L );
    //    if (pRoot && !pRoot->is_document()) delete pRoot;
    //}

    TEST_FIXTURE(LmdCompilerTestFixture, LmdCompilerFromFile)
    {
        Document doc(m_libraryScope);
        LmdCompiler compiler(m_libraryScope, &doc);
        string path = m_scores_path + "08011-paragraph.lmd";
        ImoObj* pRoot =  compiler.compile_file(path);
        CHECK( compiler.get_file_locator() == path );
        ImoDocument* pDoc = dynamic_cast<ImoDocument*>(pRoot);
        CHECK( pDoc->get_version() == "0.0" );
        CHECK( pDoc->get_num_content_items() == 1 );
        ImoParagraph* pPara = dynamic_cast<ImoParagraph*>( pDoc->get_content_item(0) );
        CHECK( pPara != nullptr );
        CHECK( pPara->get_num_items() == 1 );
        ImoTextItem* pItem = dynamic_cast<ImoTextItem*>( pPara->get_first_item() );
        CHECK( pItem->get_text() == "This is a text for this example." );

        if (pRoot && !pRoot->is_document()) delete pRoot;
    }

//    TEST_FIXTURE(LmdCompilerTestFixture, LmdCompilerFromInput)
//    {
//        Document doc(m_libraryScope);
//        LmdCompiler compiler(m_libraryScope, &doc);
//        string path = m_scores_path + "00011-empty-fill-page.lms";
//        LdpFileReader reader(path);
//        ImoObj* pRoot =  compiler.compile_input(reader);
//        CHECK( compiler.get_file_locator() == path );
//    //    ImoDocument* pDoc = dynamic_cast<ImoDocument*>(pRoot);
//    //    CHECK( pDoc->get_version() == "0.0" );
//    //    CHECK( pDoc->get_num_content_items() == 1 );
//    //    ImoScore* pScore = dynamic_cast<ImoScore*>( pDoc->get_content_item(0) );
//    //    CHECK( pScore != nullptr );
//    //    CHECK( pScore->get_id() == 3L );
//    //    CHECK( pScore->get_num_instruments() == 1 );
//    //    ImoInstrument* pInstr = pScore->get_instrument(0);
//    //    CHECK( pInstr->get_id() == 11L );
//        if (pRoot && !pRoot->is_document()) delete pRoot;
//    }

};

