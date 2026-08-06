"""Singgahan hasil pencarian dan lagu.

Memakai Redis bila REDIS_URL diisi, dan jatuh ke singgahan dalam memori proses
bila tidak. Dengan begitu server tetap berjalan pada Cloud Run yang tidak
menyediakan Redis bawaan.
"""

from __future__ import annotations

import json
import logging
import threading
import time
from typing import Any, Optional

from config import pengaturan

log = logging.getLogger("riri.cache")

_kunci_memori: dict[str, tuple[float, str]] = {}
_gembok = threading.Lock()

_redis = None
_redis_siap = False


def _klien_redis():
    global _redis, _redis_siap
    if _redis_siap:
        return _redis
    _redis_siap = True

    url = pengaturan().redis_url
    if not url:
        log.info("REDIS_URL kosong, singgahan memakai memori proses")
        return None
    try:
        import redis  # impor tertunda agar tidak wajib saat Redis tidak dipakai

        klien = redis.Redis.from_url(url, decode_responses=True, socket_timeout=3)
        klien.ping()
        _redis = klien
        log.info("Singgahan memakai Redis")
    except Exception as exc:  # pragma: no cover - bergantung lingkungan
        log.warning("Redis tidak dapat dipakai (%s), beralih ke memori proses", exc)
        _redis = None
    return _redis


def ambil(kunci: str) -> Optional[Any]:
    klien = _klien_redis()
    if klien is not None:
        try:
            data = klien.get(kunci)
            return json.loads(data) if data else None
        except Exception as exc:  # pragma: no cover
            log.warning("Gagal membaca singgahan Redis: %s", exc)
            return None

    with _gembok:
        item = _kunci_memori.get(kunci)
        if item is None:
            return None
        kedaluwarsa, data = item
        if kedaluwarsa < time.time():
            _kunci_memori.pop(kunci, None)
            return None
        return json.loads(data)


def simpan(kunci: str, nilai: Any, ttl: int) -> None:
    data = json.dumps(nilai, ensure_ascii=False)
    klien = _klien_redis()
    if klien is not None:
        try:
            klien.setex(kunci, ttl, data)
            return
        except Exception as exc:  # pragma: no cover
            log.warning("Gagal menulis singgahan Redis: %s", exc)

    with _gembok:
        _kunci_memori[kunci] = (time.time() + ttl, data)
        # Jaga agar singgahan memori tidak tumbuh tanpa batas.
        if len(_kunci_memori) > 512:
            sekarang = time.time()
            for k in [k for k, (exp, _) in _kunci_memori.items() if exp < sekarang]:
                _kunci_memori.pop(k, None)
