#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QAudioOutput>
#include <QFileInfo>
#include <QList>
#include <QMediaPlayer>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QtGlobal>

class AudioManager
{
public:
    AudioManager()
    {
        m_player.setAudioOutput(&m_audioOutput);
        setVolume(0.45f);

        QObject::connect(&m_player, &QMediaPlayer::mediaStatusChanged, &m_player,
                         [this](QMediaPlayer::MediaStatus status) {
            if (status != QMediaPlayer::EndOfMedia || m_playlist.isEmpty()) {
                return;
            }
            m_playlistIndex = (m_playlistIndex + 1) % m_playlist.size();
            m_player.setSource(m_playlist.at(m_playlistIndex));
            m_player.play();
        });
    }

    void playLoop(const QString &filePath)
    {
        if (filePath.trimmed().isEmpty()) {
            return;
        }

        m_playlist.clear();
        m_playlistIndex = 0;
        const QUrl source = QUrl::fromLocalFile(QFileInfo(filePath).absoluteFilePath());
        if (m_player.source() != source) {
            m_player.setSource(source);
        }
        m_player.setLoops(QMediaPlayer::Infinite);

        m_player.play();
    }

    void playPlaylistLoop(const QStringList &filePaths)
    {
        QList<QUrl> playlist;
        for (const QString &filePath : filePaths) {
            if (filePath.trimmed().isEmpty()) {
                continue;
            }
            playlist.append(QUrl::fromLocalFile(QFileInfo(filePath).absoluteFilePath()));
        }

        if (playlist.isEmpty()) {
            return;
        }

        if (playlist != m_playlist) {
            m_playlist = playlist;
            m_playlistIndex = 0;
            m_player.setSource(m_playlist.first());
        } else if (m_player.source().isEmpty()) {
            m_player.setSource(m_playlist.at(m_playlistIndex));
        }

        m_player.setLoops(QMediaPlayer::Once);
        m_player.play();
    }

    void pause()
    {
        if (m_player.playbackState() == QMediaPlayer::PlayingState) {
            m_player.pause();
        }
    }

    void stop()
    {
        if (m_player.playbackState() != QMediaPlayer::StoppedState) {
            m_player.stop();
        }
    }

    void setVolume(float volume)
    {
        m_audioOutput.setVolume(qBound(0.0, static_cast<double>(volume), 1.0));
    }

private:
    QMediaPlayer m_player;
    QAudioOutput m_audioOutput;
    QList<QUrl> m_playlist;
    int m_playlistIndex = 0;
};

#endif // AUDIOMANAGER_H
