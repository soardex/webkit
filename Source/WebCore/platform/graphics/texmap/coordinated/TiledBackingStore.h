/*
 Copyright (C) 2010-2012 Nokia Corporation and/or its subsidiary(-ies)
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.
 
 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
 */

#ifndef TiledBackingStore_h
#define TiledBackingStore_h

#if USE(COORDINATED_GRAPHICS)

#include "FloatPoint.h"
#include "IntPoint.h"
#include "IntRect.h"
#include "Tile.h"
#include "Timer.h"
#include <wtf/Assertions.h>
#include <wtf/HashMap.h>
#include <wtf/RefPtr.h>

namespace WebCore {

class GraphicsContext;
class TiledBackingStore;
class TiledBackingStoreClient;

class TiledBackingStore {
    WTF_MAKE_NONCOPYABLE(TiledBackingStore); WTF_MAKE_FAST_ALLOCATED;
public:
    TiledBackingStore(TiledBackingStoreClient*);
    ~TiledBackingStore();

    TiledBackingStoreClient* client() { return m_client; }

    void setTrajectoryVector(const FloatPoint&);
    void coverWithTilesIfNeeded();

    float contentsScale() { return m_contentsScale; }
    void setContentsScale(float);

    void updateTileBuffers();

    void invalidate(const IntRect& dirtyRect);

    IntSize tileSize() { return m_tileSize; }
    void setTileSize(const IntSize&);

    IntRect mapToContents(const IntRect&) const;
    IntRect mapFromContents(const IntRect&) const;

    IntRect tileRectForCoordinate(const Tile::Coordinate&) const;
    Tile::Coordinate tileCoordinateForPoint(const IntPoint&) const;
    double tileDistance(const IntRect& viewport, const Tile::Coordinate&) const;

    IntRect coverRect() const { return m_coverRect; }
    bool visibleAreaIsCovered() const;
    void removeAllNonVisibleTiles();

    void setSupportsAlpha(bool);

private:
    void createTiles();
    void computeCoverAndKeepRect(const IntRect& visibleRect, IntRect& coverRect, IntRect& keepRect) const;

    bool resizeEdgeTiles();
    void setCoverRect(const IntRect& rect) { m_coverRect = rect; }
    void setKeepRect(const IntRect&);

    PassRefPtr<Tile> tileAt(const Tile::Coordinate&) const;
    void setTile(const Tile::Coordinate&, PassRefPtr<Tile>);
    void removeTile(const Tile::Coordinate&);

    IntRect visibleRect() const;

    float coverageRatio(const IntRect&) const;
    void adjustForContentsRect(IntRect&) const;

    void paintCheckerPattern(GraphicsContext*, const IntRect&, const Tile::Coordinate&);

private:
    TiledBackingStoreClient* m_client;

    typedef HashMap<Tile::Coordinate, RefPtr<Tile> > TileMap;
    TileMap m_tiles;

    IntSize m_tileSize;
    float m_coverAreaMultiplier;

    FloatPoint m_trajectoryVector;
    FloatPoint m_pendingTrajectoryVector;
    IntRect m_visibleRect;

    IntRect m_coverRect;
    IntRect m_keepRect;
    IntRect m_rect;

    float m_contentsScale;

    bool m_supportsAlpha;
    bool m_pendingTileCreation;

    friend class Tile;
};

}

#endif
#endif
