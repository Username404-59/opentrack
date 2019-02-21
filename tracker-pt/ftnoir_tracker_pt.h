/* Copyright (c) 2012 Patrick Ruoff
 * Copyright (c) 2014-2016 Stanislaw Halik <sthalik@misaki.pl>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */

#pragma once

#include "api/plugin-api.hpp"
#include "pt-api.hpp"
#include "point_tracker.h"
#include "cv/numeric.hpp"
#include "video/video-widget.hpp"

#include <atomic>
#include <memory>
#include <vector>

#include <opencv2/core.hpp>

#include <QThread>
#include <QMutex>
#include <QLayout>

namespace pt_impl {

class TrackerDialog_PT;

using namespace numeric_types;

class Tracker_PT : public QThread, public ITracker
{
    Q_OBJECT

    friend class TrackerDialog_PT;

public:
    template<typename t> using pointer = pt_pointer<t>;

    explicit Tracker_PT(pointer<pt_runtime_traits> const& pt_runtime_traits);
    ~Tracker_PT() override;
    module_status start_tracker(QFrame* parent_window) override;
    void data(double* data) override;
    bool center() override;

    int  get_n_points();
    [[nodiscard]] bool get_cam_info(pt_camera_info& info);
    Affine pose() const;
public slots:
    bool maybe_reopen_camera();
    void set_fov(int value);
protected:
    void run() override;
private:
    pointer<pt_runtime_traits> traits;

    QMutex camera_mtx;

    PointTracker point_tracker;

    pt_settings s;

    std::unique_ptr<QLayout> layout;
    std::vector<vec2> points;

    int preview_width = 320, preview_height = 240;

    pointer<pt_point_extractor> point_extractor;
    pointer<pt_camera> camera;
    pointer<video_widget> widget;
    pointer<pt_frame> frame;
    pointer<pt_preview> preview_frame;

    std::atomic<unsigned> point_count { 0 };
    std::atomic<bool> ever_success { false };
    mutable QMutex center_lock, data_lock;
};

} // ns pt_impl

using Tracker_PT = pt_impl::Tracker_PT;
