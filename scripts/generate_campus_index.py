#!/usr/bin/env python3
import argparse
import re
from pathlib import Path


FIELD_WEIGHTS = {
    "id": 8,
    "category": 4,
    "title": 12,
    "content": 4,
    "keywords": 10,
}

STOPWORDS = {
    "dan", "atau", "yang", "untuk", "dengan", "dari", "pada", "dalam", "the",
    "of", "for", "and", "to", "di", "ke", "apa", "siapa", "berapa", "adalah",
    "jadi", "agar", "serta", "atau", "ini", "itu", "ucic", "cic", "kampus",
    "universitas",
}

SHORT_TOKEN_ALLOWLIST = {
    "ti", "si", "dkv", "akt", "mjn", "bisdi", "pkor", "pikor", "mi", "mb",
    "fti", "feb", "fps", "mku", "ai", "ui", "ux", "vr", "ar", "wa", "toefl",
    "seo", "erp", "imc", "dpp",
}


def decode_c_string(value: str) -> str:
    return bytes(value, "utf-8").decode("unicode_escape")


def normalize_text(text: str) -> str:
    text = text.lower()
    text = re.sub(r"[^a-z0-9]+", " ", text)
    return re.sub(r"\s+", " ", text).strip()


def tokenize(text: str) -> list[str]:
    normalized = normalize_text(text)
    if not normalized:
        return []

    result: list[str] = []
    for token in normalized.split():
        if token in STOPWORDS:
            continue
        if len(token) <= 1:
            continue
        if len(token) == 2 and token not in SHORT_TOKEN_ALLOWLIST and not token.isdigit():
            continue
        result.append(token)
    return result


def extract_entries(source: str) -> list[dict[str, str]]:
    marker = "static const CampusEntry CAMPUS_DB[] = {"
    start = source.find(marker)
    if start < 0:
        raise RuntimeError("CAMPUS_DB marker not found")
    end = source.find("};", start)
    if end < 0:
        raise RuntimeError("CAMPUS_DB terminator not found")
    body = source[start:end]

    pattern = re.compile(
        r'\{\s*"((?:\\.|[^"\\])*)"\s*,\s*"((?:\\.|[^"\\])*)"\s*,\s*"((?:\\.|[^"\\])*)"\s*,\s*"((?:\\.|[^"\\])*)"\s*,\s*"((?:\\.|[^"\\])*)"\s*\}',
        re.DOTALL,
    )

    entries: list[dict[str, str]] = []
    for match in pattern.finditer(body):
        entry = {
            "id": decode_c_string(match.group(1)),
            "category": decode_c_string(match.group(2)),
            "title": decode_c_string(match.group(3)),
            "content": decode_c_string(match.group(4)),
            "keywords": decode_c_string(match.group(5)),
        }
        entries.append(entry)

    if not entries:
        raise RuntimeError("No campus entries parsed")
    return entries


def build_index(entries: list[dict[str, str]]):
    vocab: dict[str, dict[int, int]] = {}
    for entry_index, entry in enumerate(entries):
        token_weights: dict[str, int] = {}
        for field_name, field_weight in FIELD_WEIGHTS.items():
            for token in tokenize(entry[field_name]):
                token_weights[token] = min(48, token_weights.get(token, 0) + field_weight)

        for token, weight in token_weights.items():
            postings = vocab.setdefault(token, {})
            postings[entry_index] = max(postings.get(entry_index, 0), weight)

    sorted_tokens = sorted(vocab.keys())
    postings_flat: list[tuple[int, int]] = []
    token_rows: list[tuple[str, int, int]] = []
    posting_offset = 0

    for token in sorted_tokens:
        postings = sorted(vocab[token].items(), key=lambda item: item[0])
        token_rows.append((token, posting_offset, len(postings)))
        postings_flat.extend(postings)
        posting_offset += len(postings)

    return token_rows, postings_flat


def escape_c_string(value: str) -> str:
    return value.replace("\\", "\\\\").replace('"', '\\"')


def write_header(output: Path, entries: list[dict[str, str]], token_rows, postings_flat):
    lines: list[str] = []
    lines.append("// Dibuat otomatis oleh scripts/generate_campus_index.py")
    lines.append("#pragma once")
    lines.append("")
    lines.append("#include <cstddef>")
    lines.append("#include <cstdint>")
    lines.append("")
    lines.append("struct CampusIndexPosting {")
    lines.append("    uint16_t entry_index;")
    lines.append("    uint8_t weight;")
    lines.append("};")
    lines.append("")
    lines.append("struct CampusIndexTokenEntry {")
    lines.append("    const char* token;")
    lines.append("    uint16_t posting_offset;")
    lines.append("    uint16_t posting_count;")
    lines.append("};")
    lines.append("")
    lines.append("static const CampusIndexPosting kCampusIndexPostings[] = {")
    for entry_index, weight in postings_flat:
        lines.append(f"    {{{entry_index}, {weight}}},")
    lines.append("};")
    lines.append("")
    lines.append("static const CampusIndexTokenEntry kCampusIndexTokens[] = {")
    for token, offset, count in token_rows:
        lines.append(f'    {{"{escape_c_string(token)}", {offset}, {count}}},')
    lines.append("};")
    lines.append("")
    lines.append("static constexpr size_t kCampusIndexTokenCount = sizeof(kCampusIndexTokens) / sizeof(kCampusIndexTokens[0]);")
    lines.append("static constexpr size_t kCampusIndexPostingCount = sizeof(kCampusIndexPostings) / sizeof(kCampusIndexPostings[0]);")
    lines.append(f"static constexpr size_t kCampusIndexEntryCount = {len(entries)};")
    lines.append("")
    output.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    source = Path(args.input).read_text(encoding="utf-8")
    entries = extract_entries(source)
    token_rows, postings_flat = build_index(entries)
    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)
    write_header(output, entries, token_rows, postings_flat)


if __name__ == "__main__":
    main()
