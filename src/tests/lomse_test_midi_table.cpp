//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Lomse is copyrighted work (c) 2010-2017. All rights reserved.
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
#include "lomse_midi_table.h"
#include "lomse_document.h"
#include "lomse_internal_model.h"
//#include "lomse_staffobjs_table.h"


using namespace UnitTest;
using namespace std;
using namespace lomse;

//---------------------------------------------------------------------------------------
//Helper, to access protected members
class MySoundEventsTable : public SoundEventsTable
{
public:
    MySoundEventsTable(ImoScore* pScore) : SoundEventsTable(pScore) {}
    virtual ~MySoundEventsTable() {}

    void my_program_sounds_for_instruments() { program_sounds_for_instruments(); }
    void my_create_events() { create_events(); }
    void my_close_table() { close_table(); }
    void my_sort_by_time() { sort_by_time(); }

};

//---------------------------------------------------------------------------------------
class MidiTableTestFixture
{
public:
    LibraryScope m_libraryScope;
    string m_scores_path;
    Document* m_pDoc;
    ImoScore* m_pScore;
    SoundEventsTable* m_pTable;

    MidiTableTestFixture()     //SetUp fixture
        : m_libraryScope(cout)
        , m_scores_path(TESTLIB_SCORES_PATH)
        , m_pDoc(nullptr)
    {
        m_scores_path = TESTLIB_SCORES_PATH;
        m_scores_path += "unit-tests/repeats/";
        m_libraryScope.set_default_fonts_path(TESTLIB_FONTS_PATH);
    }

    ~MidiTableTestFixture()    //TearDown fixture
    {
        delete m_pDoc;
        m_pDoc = nullptr;
    }

    inline const char* test_name()
    {
        return UnitTest::CurrentTest::Details()->testName;
    }

    void load_mxl_score_for_test(const std::string& score)
    {
        string filename = m_scores_path + score;
        ifstream score_file(filename.c_str());
        if (!score_file.good())
            cout  << test_name() << ". Unit test filename not found: " << filename << endl;

        m_pDoc = LOMSE_NEW Document(m_libraryScope, cout);
        m_pDoc->from_file(filename, Document::k_format_mxl);
        m_pScore = static_cast<ImoScore*>( m_pDoc->get_imodoc()->get_content_item(0) );

        m_pTable = m_pScore->get_midi_table();
    }

    void load_ldp_score_for_test(const std::string& score)
    {
        string filename = m_scores_path + score;
        ifstream score_file(filename.c_str());
        if (!score_file.good())
            cout  << test_name() << ". Unit test filename not found: " << filename << endl;

        m_pDoc = LOMSE_NEW Document(m_libraryScope, cout);
        m_pDoc->from_file(filename, Document::k_format_ldp);
        m_pScore = static_cast<ImoScore*>( m_pDoc->get_imodoc()->get_content_item(0) );

        m_pTable = m_pScore->get_midi_table();
    }

    bool is_jump1(int i, int measure, int times)
    {
        JumpEntry* pEntry = static_cast<JumpEntry*>( m_pTable->get_jump(i) );
        if (pEntry->get_measure(0) != measure
            || pEntry->get_times(0) != times
            || pEntry->get_applied(0) != 0 )
        {
            cout << test_name() << ". JumpEntry " << i << ": "
                 << pEntry->dump_entry(m_pTable);
            return false;
        }
        return true;
    }

    bool is_jump1_event(int i, int measure, int event)
    {
        JumpEntry* pEntry = static_cast<JumpEntry*>( m_pTable->get_jump(i) );
        int eventIndex = m_pTable->get_event_index(pEntry->get_event(0));
        if (pEntry->get_measure(0) != measure
            || eventIndex != event )
        {
            cout << test_name() << ". JumpEntry " << i << ": "
                 << pEntry->dump_entry(m_pTable);
            return false;
        }
        return true;
    }

    bool is_jump2(int i, int measure1, int times1, int measure2, int times2)
    {
        JumpEntry* pEntry = static_cast<JumpEntry*>( m_pTable->get_jump(i) );
        if (pEntry->get_measure(0) != measure1
            || pEntry->get_times(0) != times1
            || pEntry->get_applied(0) != 0 )
        {
            cout << test_name() << ". JumpEntry " << i << ": "
                 << pEntry->dump_entry(m_pTable);
            return false;
        }

        if (pEntry->get_measure(1) != measure2
            || pEntry->get_times(1) != times2
            || pEntry->get_applied(1) != 0 )
        {
            cout << test_name() << ". JumpEntry " << i << ": "
                 << pEntry->dump_entry(m_pTable);
            return false;
        }
        return true;
    }

    bool is_jump2_event(int i, int measure1, int event1, int measure2, int event2)
    {
        JumpEntry* pEntry = static_cast<JumpEntry*>( m_pTable->get_jump(i) );
        int eventIndex = m_pTable->get_event_index(pEntry->get_event(0));
        if (pEntry->get_measure(0) != measure1
            || eventIndex != event1 )
        {
            cout << test_name() << ". JumpEntry " << i << ": "
                 << pEntry->dump_entry(m_pTable);
            return false;
        }

        eventIndex = m_pTable->get_event_index(pEntry->get_event(1));
        if (pEntry->get_measure(1) != measure2
            || eventIndex != event2 )
        {
            cout << test_name() << ". JumpEntry " << i << ": "
                 << pEntry->dump_entry(m_pTable);
            return false;
        }
        return true;
    }

};

SUITE(MidiTableTest)
{

    TEST_FIXTURE(MidiTableTestFixture, ProgramSounds)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(n c4 q) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();

        //cout << "num.events = " << table.num_events() << endl;
        CHECK( table.num_events() == 1 );
        std::vector<SoundEvent*>& events = table.get_events();
        SoundEvent* ev = events.front();
        CHECK( ev->Channel == 0 );
        CHECK( ev->Instrument == 0 );
        CHECK( ev->EventType == SoundEvent::k_prog_instr );
    }

    TEST_FIXTURE(MidiTableTestFixture, ProgramSoundsMidiInfo)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (infoMIDI 2 0)(musicData (clef G)(n c4 q) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();

        //cout << "num.events = " << table.num_events() << endl;
        CHECK( table.num_events() == 1 );
        std::vector<SoundEvent*>& events = table.get_events();
        SoundEvent* ev = events.front();
        CHECK( ev->Channel == 0 );
        CHECK( ev->Instrument == 2 );
        CHECK( ev->EventType == SoundEvent::k_prog_instr );
    }

    TEST_FIXTURE(MidiTableTestFixture, CreateEvents_OneNote)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(n c4 q) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();
        table.my_create_events();

        CHECK( table.num_events() == 3 );
        std::vector<SoundEvent*>& events = table.get_events();
        std::vector<SoundEvent*>::iterator it = events.begin();
        CHECK( (*it)->EventType == SoundEvent::k_prog_instr );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_on );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_off );
    }

    TEST_FIXTURE(MidiTableTestFixture, CreateEvents_OneRest)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(r q) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();
        table.my_create_events();

        //cout << "num.events = " << table.num_events() << endl;
        CHECK( table.num_events() == 3 );
        std::vector<SoundEvent*>& events = table.get_events();
        std::vector<SoundEvent*>::iterator it = events.begin();
        CHECK( (*it)->EventType == SoundEvent::k_prog_instr );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_visual_on );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_visual_off );
    }

    TEST_FIXTURE(MidiTableTestFixture, CreateEvents_RestNoVisible)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(r q noVisible)(n c4 q) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();
        table.my_create_events();

        //cout << "num.events = " << table.num_events() << endl;
        CHECK( table.num_events() == 3 );
        std::vector<SoundEvent*>& events = table.get_events();
        std::vector<SoundEvent*>::iterator it = events.begin();
        CHECK( (*it)->EventType == SoundEvent::k_prog_instr );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_on );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_off );
    }

    TEST_FIXTURE(MidiTableTestFixture, CreateEvents_TwoNotesTied)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(n c4 q l)(n c4 e) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();
        table.my_create_events();

        CHECK( table.num_events() == 5 );
        std::vector<SoundEvent*>& events = table.get_events();
        std::vector<SoundEvent*>::iterator it = events.begin();
        CHECK( (*it)->EventType == SoundEvent::k_prog_instr );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_on );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_visual_off );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_visual_on );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_off );
    }

    TEST_FIXTURE(MidiTableTestFixture, BarlineIncrementsMeasureCount)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(n c4 q)(barline)(n c4 e) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();
        table.my_create_events();

        std::vector<SoundEvent*>& events = table.get_events();
        std::vector<SoundEvent*>::iterator it = events.begin();
        CHECK( (*it)->EventType == SoundEvent::k_prog_instr );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_on );
        CHECK( (*it)->Measure == 1 );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_off );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_on );
        CHECK( (*it)->Measure == 2 );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_off );
    }

    TEST_FIXTURE(MidiTableTestFixture, TimeSignatureAddsRythmChange)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(time 2 4) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();
        table.my_create_events();

        CHECK( table.num_events() == 2 );
        std::vector<SoundEvent*>& events = table.get_events();
        std::vector<SoundEvent*>::iterator it = events.begin();
        CHECK( (*it)->EventType == SoundEvent::k_prog_instr );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_rhythm_change );
        CHECK( (*it)->NumPulses == 2 );
        CHECK( (*it)->MeasureDuration == 128 );
        //cout << ", NumPulses = " << (*it)->NumPulses
        //     << ", MeasureDuration = " << (*it)->MeasureDuration << endl;
    }

    TEST_FIXTURE(MidiTableTestFixture, TimeSignatureInfoOk)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(time 6 8) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();
        table.my_create_events();

        CHECK( table.num_events() == 2 );
        std::vector<SoundEvent*>& events = table.get_events();
        std::vector<SoundEvent*>::iterator it = events.begin();
        CHECK( (*it)->EventType == SoundEvent::k_prog_instr );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_rhythm_change );
        CHECK( (*it)->NumPulses == 2 );
        CHECK( (*it)->MeasureDuration == 192 );
        //cout << ", NumPulses = " << (*it)->NumPulses
        //     << ", MeasureDuration = " << (*it)->MeasureDuration << endl;
    }

    TEST_FIXTURE(MidiTableTestFixture, CloseTableAddsEvent)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (r q) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_close_table();

        CHECK( table.num_events() == 1 );
        std::vector<SoundEvent*>& events = table.get_events();
        std::vector<SoundEvent*>::iterator it = events.begin();
        CHECK( (*it)->EventType == SoundEvent::k_end_of_score );
        CHECK( (*it)->DeltaTime == 0.0f );
    }

    TEST_FIXTURE(MidiTableTestFixture, CloseTableFinalTime)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (r q) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();
        table.my_create_events();
        table.my_close_table();

        CHECK( table.num_events() == 4 );
        std::vector<SoundEvent*>& events = table.get_events();
        std::vector<SoundEvent*>::iterator it = events.begin();
        CHECK( (*it)->EventType == SoundEvent::k_prog_instr );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_visual_on );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_visual_off );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_end_of_score );
        CHECK( (*it)->DeltaTime == 64.0f );
    }

    TEST_FIXTURE(MidiTableTestFixture, EventsSorted)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(chord (n c4 q)(n e4 q)) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.my_program_sounds_for_instruments();
        table.my_create_events();
        table.my_close_table();
        table.my_sort_by_time();

        std::vector<SoundEvent*>& events = table.get_events();
        std::vector<SoundEvent*>::iterator it = events.begin();
        CHECK( (*it)->EventType == SoundEvent::k_prog_instr );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_on );
        CHECK( (*it)->DeltaTime == 0.0f );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_on );
        CHECK( (*it)->DeltaTime == 0.0f );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_off );
        CHECK( (*it)->DeltaTime == 64.0f );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_note_off );
        CHECK( (*it)->DeltaTime == 64.0f );
        ++it;
        CHECK( (*it)->EventType == SoundEvent::k_end_of_score );
        CHECK( (*it)->DeltaTime == 64.0f );
    }


    //@ Measures table ------------------------------------------------------------------

    TEST_FIXTURE(MidiTableTestFixture, MeasuresTable)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(chord (n c4 q)(n e4 q)) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.create_table();

        std::vector<SoundEvent*>& events = table.get_events();

        int iEv = table.get_first_event_for_measure(1);
        CHECK( iEv == 1 );
        CHECK( events[iEv]->EventType == SoundEvent::k_note_on );

        iEv = table.get_last_event();
        CHECK( iEv == 5 );
        CHECK( events[iEv]->EventType == SoundEvent::k_end_of_score );

        CHECK( table.get_num_measures() == 1 );
    }

    TEST_FIXTURE(MidiTableTestFixture, MeasuresTable_ExtraFinalMeasure)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(chord (n c4 q)(n e4 q)) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.create_table();

        std::vector<SoundEvent*>& events = table.get_events();

        int iEv = table.get_first_event_for_measure(2);
        CHECK( iEv == 5 );
        CHECK( events[iEv]->EventType == SoundEvent::k_end_of_score );
    }

    TEST_FIXTURE(MidiTableTestFixture, MeasuresTable_InitialControlMeasure)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(chord (n c4 q)(n e4 q)) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.create_table();

        std::vector<SoundEvent*>& events = table.get_events();

        int iEv = table.get_first_event_for_measure(0);
        CHECK( iEv == 0 );
        CHECK( events[iEv]->EventType == SoundEvent::k_prog_instr );
    }

    TEST_FIXTURE(MidiTableTestFixture, MeasuresTable_TwoMeasures)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(n c4 q)(barline)(n c4 e) )) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );
        MySoundEventsTable table(pScore);
        table.create_table();

        std::vector<SoundEvent*>& events = table.get_events();

        int iEv = table.get_first_event_for_measure(0);
        CHECK( iEv == 0 );
        CHECK( events[iEv]->EventType == SoundEvent::k_prog_instr );

        iEv = table.get_first_event_for_measure(1);
        CHECK( iEv == 1 );
        CHECK( events[iEv]->EventType == SoundEvent::k_note_on );

        iEv = table.get_first_event_for_measure(2);
        CHECK( iEv == 3 );
        CHECK( events[iEv]->EventType == SoundEvent::k_note_on );

        iEv = table.get_first_event_for_measure(3);
        CHECK( iEv == 5 );
        CHECK( events[iEv]->EventType == SoundEvent::k_end_of_score );

        iEv = table.get_last_event();
        CHECK( iEv == 5 );
        CHECK( events[iEv]->EventType == SoundEvent::k_end_of_score );

        CHECK( table.get_num_measures() == 2 );
    }


    //@ Jumps table ------------------------------------------------------------------

    TEST_FIXTURE(MidiTableTestFixture, jumps_table_01)
    {
        //@001. empty score creates empty table
        Document doc(m_libraryScope);
        doc.from_string("(score (vers 2.0)(instrument (musicData )))");
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );

		SoundEventsTable* pTable = pScore->get_midi_table();

        CHECK( pTable->num_jumps() == 0 );
    }

    TEST_FIXTURE(MidiTableTestFixture, jumps_table_02)
    {
        //@002. score with no repetitions creates empty table
        Document doc(m_libraryScope);
        doc.from_string("(score (vers 2.0)(instrument (musicData "
            "(clef G)(n c4 q)(n e4 q)(n g4 q) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );

		SoundEventsTable* pTable = pScore->get_midi_table();

        CHECK( pTable->num_jumps() == 0 );
    }

    TEST_FIXTURE(MidiTableTestFixture, jumps_table_03)
    {
        //@003. [T1] end-repetition barline creates one repetition
        //  |    |    |    :|     |     |
        //  1    2    3     4     5
        //                 J 2,1
        load_mxl_score_for_test("01-repeat-end-repetition-barline.xml");
        CHECK( m_pTable->num_jumps() == 1 );
        CHECK( is_jump1(0, 1,1) == true );
        CHECK( is_jump1_event(0, 1,1) == true );
        //cout << m_pTable->dump_midi_events() << endl;
    }

    TEST_FIXTURE(MidiTableTestFixture, jumps_table_04)
    {
        //@004. [T2] start-end-repetition barlines creates one repetition
        //  |    |     |:    |    :|     |     |
        //  1    2     3     4     5     6
        //                       J 3,1
        load_mxl_score_for_test("02-repeat-start-end-repetition-barlines.xml");
        CHECK( m_pTable->num_jumps() == 1 );
        CHECK( is_jump1(0, 3,1) == true );
        CHECK( is_jump1_event(0, 3,6) == true );
        //cout << m_pTable->dump_midi_events() << endl;
    }

    TEST_FIXTURE(MidiTableTestFixture, jumps_table_05)
    {
        //@005. As 004 but with LDP score
        //  |    |:    |    :|     |     |
        //  1    2     3     4     5
        //                 J 2,1
        Document doc(m_libraryScope, cout);
        doc.from_string("(score (vers 2.0)(instrument (musicData "
            "(clef G)(n c4 q)(n e4 q)(barline startRepetition)"
            "(n c4 q)(n e4 q)(barline)"
            "(n c4 q)(n e4 q)(barline endRepetition)"
            "(n c4 q)(n e4 q)(barline)(n c4 q)(n e4 q)(barline) )))" );
        ImoScore* pScore = static_cast<ImoScore*>( doc.get_imodoc()->get_content_item(0) );

		m_pTable = pScore->get_midi_table();

        CHECK( m_pTable->num_jumps() == 1 );
        CHECK( is_jump1(0, 2,1) == true );
        CHECK( is_jump1_event(0, 2,5) == true );
        //cout << m_pTable->dump_midi_events() << endl;
    }

    TEST_FIXTURE(MidiTableTestFixture, jumps_table_06)
    {
        //@006. [T3] start-end-repetition barlines creates one repetition
        //  |    |    :|:    |    :|     |     |
        //  1    2     3     4     5     6
        //            J 1,1       J 3,1
        load_mxl_score_for_test("03-repeat-double-end-repetition-barlines.xml");
        CHECK( m_pTable->num_jumps() == 2 );
        CHECK( is_jump1(0, 1,1) == true );
        CHECK( is_jump1_event(0, 1,1) == true );
        CHECK( is_jump1(1, 3,1) == true );
        CHECK( is_jump1_event(1, 3,6) == true );
        //cout << m_pTable->dump_midi_events() << endl;
    }

    TEST_FIXTURE(MidiTableTestFixture, jumps_table_07)
    {
        //@007. [T4] end-repetition barline with volta brackets
        //                  vt1    vt2
        //  |    |    |     |     :|     |     |     |
        //  1    2    3     4      5    6
        //                 J 4,1  J 1,1
        //                   5,0
        load_mxl_score_for_test("04-repeat-barline-simple-volta.xml");
        cout << m_pTable->dump_midi_events() << endl;
        //cout << m_pScore->get_staffobjs_table()->dump() << endl;
        CHECK( m_pTable->num_jumps() == 2 );
        CHECK( is_jump2(0, 4,1, 5,0) == true );
        CHECK( is_jump2_event(0, 4,9, 5,11) == true );
        CHECK( is_jump1(1, 1,1) == true );
        CHECK( is_jump1_event(1, 1,1) == true );
    }

}


