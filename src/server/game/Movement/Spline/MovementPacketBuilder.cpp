/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2014 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "MovementPacketBuilder.h"
#include "MoveSpline.h"
#include "WorldPacket.h"
#include "Object.h"
#include "Unit.h"

namespace Movement
{
    inline void operator << (ByteBuffer& b, const Vector3& v)
    {
        b << v.x << v.y << v.z;
    }

    inline void operator >> (ByteBuffer& b, Vector3& v)
    {
        b >> v.x >> v.y >> v.z;
    }

    MonsterMoveType PacketBuilder::GetMonsterMoveType(MoveSpline const& moveSpline)
    {
        switch (moveSpline.splineflags & MoveSplineFlag::Mask_Final_Facing)
        {
            case MoveSplineFlag::Final_Target:
                return MonsterMoveFacingTarget;
            case MoveSplineFlag::Final_Angle:
                return MonsterMoveFacingAngle;
            case MoveSplineFlag::Final_Point:
                return MonsterMoveFacingPoint;
            default:
                return MonsterMoveNormal;
        }
    }

    void PacketBuilder::WriteStopMovement(Vector3 const& pos, uint32 splineId, ByteBuffer& data, Unit* unit)
    {

        ObjectGuid guid = unit->GetGUID();
        ObjectGuid transport = unit->GetTransGUID();

        data << float(pos.z);
        data << float(pos.x);
        data << uint32(splineId);
        data << float(pos.y);
        data << float(0.f); // Most likely transport Y
        data << float(0.f); // Most likely transport Z
        data << float(0.f); // Most likely transport X

        data.WriteBit(1); // Parabolic speed // esi+4Ch
        data.WriteBit(guid[0]);
        data.WriteBits(MonsterMoveStop, 3);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBits(0,  20);
        data.WriteBit(1);
        data.WriteBit(guid[3]);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteGuidMask(guid, 7, 4);
        data.WriteBit(1);
        data.WriteBit(guid[5]);
        data.WriteBits(0, 22); // WP count
        data.WriteBit(guid[6]);
        data.WriteBit(0); // Fake bit
        data.WriteGuidMask(transport, 7, 1, 3, 0, 6, 4, 5, 2);
        data.WriteBit(0); // Send no block
        data.WriteBit(0);
        data.WriteGuidMask(guid, 2, 1);

        data.FlushBits();

        data.WriteByteSeq(guid[1]);
        data.WriteGuidBytes(transport, 6, 4, 1, 7, 0, 3, 5, 2);
        data.WriteGuidBytes(guid, 5, 3, 6, 0, 7, 2, 4);
    }

    void WriteLinearPath(Spline<int32> const& spline, ByteBuffer& data)
    {
        uint32 last_idx = spline.getPointCount() - 3;
        Vector3 const* real_path = &spline.getPoint(1);

        if (last_idx > 0)
        {
            Vector3 middle = (real_path[0] + real_path[last_idx]) / 2.f;
            Vector3 offset;
            // first and last points already appended
            for (uint32 i = 0; i < last_idx; ++i)
            {
                offset = middle - real_path[i];
                data.appendPackXYZ(offset.x, offset.y, offset.z);
            }
        }
    }

    void WriteUncompressedPath(Spline<int32> const& spline, ByteBuffer& data)
    {
		for (int i = 1; i < spline.getPointCount() - 1; i++)
            data << spline.getPoint(i).y << spline.getPoint(i).x << spline.getPoint(i).z;
    }

    void WriteUncompressedCyclicPath(Spline<int32> const& spline, ByteBuffer& data)
    {
        data << spline.getPoint(1).y << spline.getPoint(1).x << spline.getPoint(1).z; // Fake point, client will erase it from the spline after first cycle done

        for (int i = 1; i < spline.getPointCount() - 3; i++)
            data << spline.getPoint(i).y << spline.getPoint(i).x << spline.getPoint(i).z;
    }

    void PacketBuilder::WriteMonsterMove(const MoveSpline& moveSpline, WorldPacket& data, Unit* unit)
    {
        ObjectGuid guid = unit->GetGUID();
        ObjectGuid transport = unit->GetTransGUID();
        MonsterMoveType type = GetMonsterMoveType(moveSpline);
        G3D::Vector3 const& firstPoint = moveSpline.spline.getPoint(moveSpline.spline.first());

        data << float(firstPoint.z);
        data << float(firstPoint.x);
        data << uint32(moveSpline.GetId());
        data << float(firstPoint.y);
        data << float(0.f); // Most likely transport Y
        data << float(0.f); // Most likely transport Z
        data << float(0.f); // Most likely transport X

        data.WriteBit(1); // Parabolic speed // esi+4Ch
        data.WriteBit(guid[0]);
        data.WriteBits(type, 3);

        if (type == MonsterMoveFacingTarget)
        {
            ObjectGuid targetGuid = moveSpline.facing.target;
            data.WriteGuidMask(targetGuid, 6, 4, 3, 0, 5, 7, 1, 2);
        }

        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);

		uint32 uncompressedSplineCount = moveSpline.splineflags & MoveSplineFlag::UncompressedPath ? moveSpline.splineflags.cyclic ? moveSpline.spline.getPointCount() - 3 : moveSpline.spline.getPointCount() - 2 : 1;
        data.WriteBits(uncompressedSplineCount,  20);

        data.WriteBit(!moveSpline.splineflags.raw());
        data.WriteBit(guid[3]);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(!moveSpline.Duration());
        data.WriteGuidMask(guid, 7, 4);
        data.WriteBit(1);
        data.WriteBit(guid[5]);

        int32 compressedSplineCount = moveSpline.splineflags & MoveSplineFlag::UncompressedPath ? 0 : moveSpline.spline.getPointCount() - 3;
        data.WriteBits(compressedSplineCount, 22); // WP count

        data.WriteBit(guid[6]);
        data.WriteBit(0); // Fake bit

        data.WriteGuidMask(transport, 7, 1, 3, 0, 6, 4, 5, 2);

        data.WriteBit(0); // Send no block
        data.WriteBit(0);
        data.WriteGuidMask(guid, 2, 1);

        data.FlushBits();

        if (compressedSplineCount)
            WriteLinearPath(moveSpline.spline, data);

        data.WriteByteSeq(guid[1]);
        data.WriteGuidBytes(transport, 6, 4, 1, 7, 0, 3, 5, 2);

        if (moveSpline.splineflags & MoveSplineFlag::UncompressedPath)
        {
            if (moveSpline.splineflags.cyclic)
                WriteUncompressedCyclicPath(moveSpline.spline, data);
            else
                WriteUncompressedPath(moveSpline.spline, data);
        }
        else
        {
            G3D::Vector3 const& point = moveSpline.spline.getPoint(moveSpline.spline.getPointCount() - 2);
            data << point.y << point.x << point.z;
        }

        if (type == MonsterMoveFacingTarget)
        {
            ObjectGuid targetGuid = moveSpline.facing.target;
            data.WriteGuidBytes(targetGuid, 5, 7, 0, 4, 3, 2, 6, 1);
        }

        data.WriteByteSeq(guid[5]);

        if (type == MonsterMoveFacingAngle)
            data << float(moveSpline.facing.angle);

        data.WriteByteSeq(guid[3]);

        if (moveSpline.splineflags.raw())
            data << uint32(moveSpline.splineflags.raw());

        data.WriteByteSeq(guid[6]);

        if (type == MonsterMoveFacingPoint)
            data << moveSpline.facing.f.x << moveSpline.facing.f.y << moveSpline.facing.f.z;

        data.WriteGuidBytes(guid, 0, 7, 2, 4);

        if (moveSpline.Duration())
            data << uint32(moveSpline.Duration());
    }

    void PacketBuilder::WriteCreateBits(MoveSpline const& moveSpline, ByteBuffer& data)
    {
        if (!data.WriteBit(!moveSpline.Finalized()))
            return;

        data.WriteBit(moveSpline.splineflags & (MoveSplineFlag::Parabolic | MoveSplineFlag::Animation));
        data.WriteBit((moveSpline.splineflags & MoveSplineFlag::Parabolic) && moveSpline.effect_start_time < moveSpline.Duration());
        data.WriteBit(0); // NYI Block
        data.WriteBits(moveSpline.getPath().size(), 20);
        data.WriteBits(moveSpline.spline.mode(), 2);
        data.WriteBits(moveSpline.splineflags.raw(), 25);
    }

    void PacketBuilder::WriteCreateData(MoveSpline const& moveSpline, ByteBuffer& data)
    {
        if (!moveSpline.Finalized())
        {
            MoveSplineFlag const& splineFlags = moveSpline.splineflags;
            MonsterMoveType type = GetMonsterMoveType(moveSpline);

            data << moveSpline.timePassed();
            data << float(1.f);                             // splineInfo.duration_mod_next; added in 3.1
            data << float(1.f);                             // splineInfo.duration_mod; added in 3.1

            uint32 nodes = moveSpline.getPath().size();
            for (uint32 i = 0; i < nodes; ++i)
            {
                data << float(moveSpline.getPath()[i].x);
                data << float(moveSpline.getPath()[i].z);
                data << float(moveSpline.getPath()[i].y);
            }

            if (splineFlags & (MoveSplineFlag::Parabolic | MoveSplineFlag::Animation))
                data << moveSpline.effect_start_time;       // added in 3.1

            data << uint8(type);

            if (type == MonsterMoveFacingAngle)
                data << float(moveSpline.facing.angle);

            if (type == MonsterMoveFacingPoint)
                data << moveSpline.facing.f.x << moveSpline.facing.f.z << moveSpline.facing.f.y;

            if ((splineFlags & MoveSplineFlag::Parabolic) && moveSpline.effect_start_time < moveSpline.Duration())
                data << float(moveSpline.vertical_acceleration);   // added in 3.1

            //    NYI block here
            data << moveSpline.Duration();

        }

        Vector3 destination = moveSpline.isCyclic() ? Vector3::zero() : moveSpline.FinalDestination();

        data << float(destination.x);
        data << float(destination.z);
        data << moveSpline.GetId();
        data << float(destination.y);

    }

    void PacketBuilder::WriteFacingTargetPart(MoveSpline const& moveSpline, ByteBuffer& data)
    {
        if (GetMonsterMoveType(moveSpline) == MonsterMoveFacingTarget && !moveSpline.Finalized())
        {
            ObjectGuid facingGuid = moveSpline.facing.target;
            data.WriteGuidMask(facingGuid, 4, 7, 0, 5, 1, 2, 3, 6);

            data.WriteGuidBytes(facingGuid, 4, 2, 0, 5, 6, 3, 1, 7);
        }
    }
}
