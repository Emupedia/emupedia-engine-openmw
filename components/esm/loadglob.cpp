#include "loadglob.hpp"

#include "esmreader.hpp"
#include "esmwriter.hpp"
#include "defs.hpp"
#include "util.hpp"

namespace ESM
{
    unsigned int Global::sRecordId = REC_GLOB;

    void Global::load (ESMReader &esm)
    {
        mId = esm.getHNString("NAME");
        if (mIsDeleted = readDeleSubRecord(esm))
        {
            return;
        }

        mValue.read (esm, ESM::Variant::Format_Global);
    }

    void Global::save (ESMWriter &esm) const
    {
        esm.writeHNCString("NAME", mId);
        if (mIsDeleted)
        {
            writeDeleSubRecord(esm);
            return;
        }

        mValue.write (esm, ESM::Variant::Format_Global);
    }

    void Global::blank()
    {
        mValue.setType (ESM::VT_None);
    }

    bool operator== (const Global& left, const Global& right)
    {
        return left.mId==right.mId && left.mValue==right.mValue;
    }
}
