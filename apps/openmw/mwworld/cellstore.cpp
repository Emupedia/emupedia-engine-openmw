#include "cellstore.hpp"

#include <iostream>

#include <components/esm_store/store.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"

#include "ptr.hpp"

namespace MWWorld
{
    CellStore::CellStore (const ESM::Cell *cell_) : cell (cell_), mState (State_Unloaded)
    {
        mWaterLevel = cell->mWater;
    }

    void CellStore::load (const ESMS::ESMStore &store, ESM::ESMReader &esm)
    {
        if (mState!=State_Loaded)
        {
            if (mState==State_Preloaded)
                mIds.clear();

            std::cout << "loading cell " << cell->getDescription() << std::endl;

            loadRefs (store, esm);

            mState = State_Loaded;
        }
    }

    void CellStore::preload (const ESMS::ESMStore &store, ESM::ESMReader &esm)
    {
        if (mState==State_Unloaded)
        {
            listRefs (store, esm);

            mState = State_Preloaded;
        }
    }

    void CellStore::listRefs(const ESMS::ESMStore &store, ESM::ESMReader &esm)
    {
        assert (cell);

        if (cell->mContextList.size() == 0)
            return; // this is a dynamically generated cell -> skipping.

        // Load references from all plugins that do something with this cell.
        // HACK: only use first entry for now, full support requires some more work
        //for (int i = 0; i < cell->mContextList.size(); i++)
        for (int i = 0; i < 1; i++)
        {
            // Reopen the ESM reader and seek to the right position.
            // TODO: we will need to intoduce separate "esm"s, one per plugin!
            cell->restore (esm);

            ESM::CellRef ref;

            // Get each reference in turn
            while (cell->getNextRef (esm, ref))
            {
                std::string lowerCase;

                std::transform (ref.mRefID.begin(), ref.mRefID.end(), std::back_inserter (lowerCase),
                    (int(*)(int)) std::tolower);

                // TODO: support deletion / moving references out of the cell. no simple "push_back",
                //  but see what the plugin wants to do.
                mIds.push_back (lowerCase);
            }
        }

        std::sort (mIds.begin(), mIds.end());
    }

    void CellStore::loadRefs(const ESMS::ESMStore &store, ESM::ESMReader &esm)
    {
      assert (cell);

        if (cell->mContextList.size() == 0)
            return; // this is a dynamically generated cell -> skipping.

        // Load references from all plugins that do something with this cell.
        // HACK: only use first entry for now, full support requires some more work
        //for (int i = 0; i < cell->mContextList.size(); i++)
        for (int i = 0; i < 1; i++)
        {
            // Reopen the ESM reader and seek to the right position.
            // TODO: we will need to intoduce separate "esm"s, one per plugin!
            cell->restore(esm);

            ESM::CellRef ref;

            // Get each reference in turn
            while(cell->getNextRef(esm, ref))
            {
                std::string lowerCase;

                std::transform (ref.mRefID.begin(), ref.mRefID.end(), std::back_inserter (lowerCase),
                    (int(*)(int)) std::tolower);

                int rec = store.find(ref.mRefID);

                ref.mRefID = lowerCase;

            /* We can optimize this further by storing the pointer to the
                record itself in store.all, so that we don't need to look it
                up again here. However, never optimize. There are infinite
                opportunities to do that later.
            */
            switch(rec)
                {
                case ESM::REC_ACTI: activators.find(ref, store.activators); break;
                case ESM::REC_ALCH: potions.find(ref, store.potions); break;
                case ESM::REC_APPA: appas.find(ref, store.appas); break;
                case ESM::REC_ARMO: armors.find(ref, store.armors); break;
                case ESM::REC_BOOK: books.find(ref, store.books); break;
                case ESM::REC_CLOT: clothes.find(ref, store.clothes); break;
                case ESM::REC_CONT: containers.find(ref, store.containers); break;
                case ESM::REC_CREA: creatures.find(ref, store.creatures); break;
                case ESM::REC_DOOR: doors.find(ref, store.doors); break;
                case ESM::REC_INGR: ingreds.find(ref, store.ingreds); break;
                case ESM::REC_LEVC: creatureLists.find(ref, store.creatureLists); break;
                case ESM::REC_LEVI: itemLists.find(ref, store.itemLists); break;
                case ESM::REC_LIGH: lights.find(ref, store.lights); break;
                case ESM::REC_LOCK: lockpicks.find(ref, store.lockpicks); break;
                case ESM::REC_MISC: miscItems.find(ref, store.miscItems); break;
                case ESM::REC_NPC_: npcs.find(ref, store.npcs); break;
                case ESM::REC_PROB: probes.find(ref, store.probes); break;
                case ESM::REC_REPA: repairs.find(ref, store.repairs); break;
                case ESM::REC_STAT: statics.find(ref, store.statics); break;
                case ESM::REC_WEAP: weapons.find(ref, store.weapons); break;

                case 0: std::cout << "Cell reference " + ref.mRefID + " not found!\n"; break;
                default:
                std::cout << "WARNING: Ignoring reference '" << ref.mRefID << "' of unhandled type\n";
                }
            }
        }
    }
}
