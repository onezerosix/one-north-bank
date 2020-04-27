// =============================================================================
// File: ral.h
// =============================================================================
// Description:
//      This header file hosts the random access library (ral) namespace.
// =============================================================================

#ifndef RAL_H
#define RAL_H

namespace ral {
    using namespace std;

    // === Record ==============================================================
    // This abstract class is the base for records in the random access file.
    // =========================================================================
    class Record {
    public:
        // === getId ===========================================================
        // Parameters: None
        //
        // Return val:
        //      int representing the id the of the record
        // =====================================================================
        virtual int getId() = 0;

        // === getSize =========================================================
        // Parmeters: None
        //
        // Return val:
        //      size_t equal to the size that a serialized record would be (in
        //      bytes)
        // =====================================================================
        virtual size_t getSize() = 0;

        // === serialize =======================================================
        // Parmeters:
        //      ss [IN/OUT]             -- stream where the serialized record
        //                                  will be written to
        //
        // Return val:
        //      true if able to serialize, otherwise false
        // =====================================================================
        virtual bool serialize(stringstream &ss) = 0;

        // === deserialize =====================================================
        // Parmeters:
        //      ss [IN/OUT]             -- stream where a serialized record will
        //                                  be read from
        //
        // Return val:
        //      true if able to deserialize, otherwise false
        // =====================================================================
        virtual bool deserialize(stringstream &ss) = 0;
    };

    // === File ================================================================
    // This class controls a random access file of 100 records. Private
    // functions do not validate that the input is valid.
    // =========================================================================
    class File {
    private:
        static const int MAX_RECORDS = 100; // TODO: move to be adjustable
        bitset<MAX_RECORDS> available_ids;
        unique_ptr<Record> dummy_record;
        size_t record_size;

        const string FILE_EXTENSION = ".raf"; // TODO: make static?
        string file_name;
        fstream file;

        // ==== reserveId ======================================================
        // Sets an id to unavailable.
        //
        // Parameters:
        //      id [IN]                 -- id to be reserved
        //
        // Return val:
        //      true if successful, otherwise false
        // =====================================================================
        bool reserveId(int id);

        // ==== releaseId ======================================================
        // Sets an id to available.
        //
        // Parameters:
        //      id [IN]                 -- id to be released
        //
        // Return val: None // TODO: return boolean?
        // =====================================================================
        void releaseId(int id);

        // ==== updateFile =====================================================
        // Parameters:
        //      id [IN]                 -- id of the record to be updated
        //      record [IN]             -- pointer to the updated record
        //      update_available_ids [OPT IN]
        //                              -- optional: boolean indicating whether 
        //                                  there the set of available_ids needs
        //                                  to be updated in the RAF. defaults
        //                                  to false
        //
        // Return val: None // TODO: bool return?
        // =====================================================================
        void updateFile(int id, Record* record,
            bool update_available_ids = false);

        // === calculateOffset =================================================
        // This function calculates where a record should be in the raf.
        //
        // Parameters:
        //      id [IN]                 -- the id of the record
        //      include_available_ids [OPT IN]  
        //                              -- optional: boolean indicating whether 
        //                                  the set located at the beginning of
        //                                  the RAF should be accounted for in
        //                                  the offset. defaults to true
        //
        // Return val:
        //      int representing the offset of the record in bytes
        // =============================================================================
        int calculateOffset(int id, bool include_available_ids = true);

    public:
        // === File ============================================================
        // This is the constructor. It creates a new raf or loads an existing
        // raf.
        //
        // Parameters:
        //      file_name [VAL]         -- name of the raf (minus extension)
        //      dummy_record [REF]      -- a dummy record to be given to this
        //                                  class
        //
        // Return value: None
        // =====================================================================
        File(string file_name, unique_ptr<Record> dummy_record);

        // ==== getNextAvailableId =============================================
        // Parameters: None
        //
        // Return val:
        //      int representing the next id if one was available, otherwise -1
        // =====================================================================
        int getNextAvailableId();

        // ==== createRecord ===================================================
        // Adds a new record to the RAF if there is room for one.
        //
        // Parameters:
        //      record [IN]             -- pointer to the record to be added
        //
        // Return val:
        //      true if the record was added, otherwise false
        // =====================================================================
        bool createRecord(Record* record);

        // === deleteRecord ====================================================
        // Deletes a record from the raf if it's up to date.
        //
        // Parameters:
        //      record [IN]             -- pointer to the record to be deleted
        //
        // Return val:
        //      true if successful, otherwise false
        // =====================================================================
        bool deleteRecord(Record* record);

        // ==== getRecord ======================================================
        // Attempts to get a record from the RAF.
        //
        // Parameters:
        //      id [IN]                 -- id of the record to get
        //      record [IN/OUT]         -- pointer to where the record should
        //                                  get written to
        //
        // Return val:
        //      true if able to get the record, otherwise false
        // =====================================================================
        bool getRecord(int id, Record* record);

        // ==== updateRecord ===================================================
        // Parameters:
        //      record [IN]             -- pointer to the updated record
        //
        // Return val: None
        // =====================================================================
        void updateRecord(Record* record);
    };
}

#endif // RAL_H