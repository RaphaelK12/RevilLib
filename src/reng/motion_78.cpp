/*  Revil Format Library
    Copyright(C) 2017-2020 Lukas Cone

    This program is free software : you can redistribute it and / or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.If not, see <https://www.gnu.org/licenses/>.
*/

#include "motion_78.hpp"

template<> int REMotion78::Fixup() {
  char *masterBuffer = reinterpret_cast<char *>(this);
  tracks.Fixup(masterBuffer);
  unkOffset02.Fixup(masterBuffer);
  animationName.Fixup(masterBuffer);

  for (uint32 b = 0; b < numTracks; b++)
    tracks[b].Fixup(masterBuffer);

  return 0;
}

int REMotionTrack78::Fixup(char *masterBuffer) {
  curves.Fixup(masterBuffer);

  uint32 numUsedCurves = 0;

  for (uint32 t = 0; t < 3; t++)
    if (usedCurves[static_cast<REMotionTrack43::TrackType>(t)])
      curves[numUsedCurves++].Fixup(masterBuffer);

  return 0;
}

int RETrackCurve78::Fixup(char *masterBuffer) {
  frames.Fixup(masterBuffer);
  controlPoints.Fixup(masterBuffer);
  minMaxBounds.Fixup(masterBuffer);

  return 0;
}

void REMotion78Asset::Build() {
  const uint32 numTracks = Get().numTracks;

  for (uint32 t = 0; t < numTracks; t++) {
    auto tck = Get().tracks.operator->() + t;
    size_t curCurve = 0;

    if (tck->usedCurves[REMotionTrack43::TrackType_Position]) {
      auto *wk = new REMotionTrackWorker();
      auto data = &tck->curves[curCurve++];
      wk->controller =
          std::unique_ptr<RETrackController>(data->GetController());
      wk->cType = REMotionTrackWorker::Position;
      wk->boneHash = tck->boneHash;
      wk->numFrames = data->numFrames;
      storage.emplace_back(wk);
    }

    if (tck->usedCurves[REMotionTrack43::TrackType_Rotation]) {
      auto *wk = new REMotionTrackWorker();
      auto data = &tck->curves[curCurve++];
      wk->controller =
          std::unique_ptr<RETrackController>(data->GetController());
      wk->cType = REMotionTrackWorker::Rotation;
      wk->boneHash = tck->boneHash;
      wk->numFrames = data->numFrames;
      storage.emplace_back(wk);
    }

    if (tck->usedCurves[REMotionTrack43::TrackType_Scale]) {
      auto *wk = new REMotionTrackWorker();
      auto data = &tck->curves[curCurve++];
      wk->controller =
          std::unique_ptr<RETrackController>(data->GetController());
      wk->cType = REMotionTrackWorker::Scale;
      wk->boneHash = tck->boneHash;
      wk->numFrames = data->numFrames;
      storage.emplace_back(wk);
    }
  }
}

int REMotion78Asset::Fixup() {
  int retVal = Get().Fixup();
  Build();

  return retVal;
}