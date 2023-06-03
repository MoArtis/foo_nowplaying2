#include <SDK/foobar2000.h>

#include "preferences.h"

class NowPlaying : public play_callback_static
{
public:
    void SetScript(const pfc::string8& script);

    pfc::string8 GetPlaybackString()
    {
        pfc::mutexScope reader(thread_lock_);
        return playback_string_;
    }

private:
    // Playback callback methods.
    unsigned get_flags() override
    {
        return flag_on_playback_new_track |
            flag_on_playback_pause |
            flag_on_playback_stop |
            flag_on_playback_seek |
            flag_on_playback_time;
    }

    void on_playback_starting(play_control::t_track_command p_command, bool p_paused) override
    {
        Update("starting");
    }

    void on_playback_new_track(metadb_handle_ptr p_track) override
    {
        Update("new track");
    }

    void on_playback_stop(play_control::t_stop_reason p_reason) override
    {
        Update("stop");
    }

    void on_playback_seek(double p_time) override
    {
        Update("seek");
    }

    void on_playback_pause(bool p_state) override
    {
        Update("pause");
    }

    void on_playback_time(double p_time) override
    {
        Update("time");
    }

    void on_playback_edited(metadb_handle_ptr p_track) override {}
    void on_playback_dynamic_info(const file_info &p_info) override {}
    void on_playback_dynamic_info_track(const file_info &p_info) override {}
    void on_volume_change(float p_new_val) override {}

    void Update(const char* event);

    pfc::mutex thread_lock_;

    pfc::string8 playback_string_;

    titleformat_object::ptr script_;
};

void NowPlaying::SetScript(const pfc::string8& script)
{
    titleformat_compiler::get()->compile(script_, script.c_str());
}

void NowPlaying::Update(const char* event)
{
    SetScript(playback_format.get());

    pfc::mutexScope writer(thread_lock_);
    playback_control::get()->playback_format_title(nullptr, playback_string_, script_, nullptr,
                                                   playback_control::display_level_all);
    // console::printf("nowplaying2 %s - %s", event, playback_string_.c_str());

    // This is how one gets a wchar_t string.
    const pfc::stringcvt::string_wide_from_utf8_t buffer(playback_string_);
}


service_factory_single_t<NowPlaying> g_nowplaying2;
