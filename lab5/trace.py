#!/usr/bin/env python3
"""
Псевдо-трасса MPI-программы 1.c (звёздная рассылка от rank 0) на size=6.
Рисуется в стиле Jumpshot: цветные блоки MPI_Send/MPI_Recv,
белые блоки MPI_Comm_rank/size, оранжевый MPI_Finalize,
жёлтые стрелки-message между процессами.
"""

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.lines import Line2D
from matplotlib.patches import FancyArrowPatch

# ---------------------------------------------------------------------------
# Параметры симуляции
# ---------------------------------------------------------------------------
SIZE = 6                     # число процессов
T_MPI_INIT   = 0.008         # MPI_Init
T_COMM_RANK  = 0.001         # MPI_Comm_rank
T_COMM_SIZE  = 0.001         # MPI_Comm_size
T_SEND_SMALL = 0.004         # передача 10 int (MPI_Send)
T_FINALIZE   = 0.006         # MPI_Finalize
T_PRINTF     = 0.002         # printf у ненулевых рангов

# Цвета — как в Jumpshot
C_SEND   = "#0000ff"   # синий
C_RECV   = "#00ff00"   # зелёный
C_RANK   = "#ffffff"   # белый (с чёрной рамкой)
C_SIZE   = "#ffffff"   # белый (с чёрной рамкой)
C_FINAL  = "#ff9900"   # оранжевый
C_INIT   = "#cccccc"   # серый — MPI_Init
C_PRINTF = "#ffcc99"   # светло-оранжевый — printf
C_MSG    = "#c8a000"   # жёлтая стрелка


# ---------------------------------------------------------------------------
# Построение модели выполнения
# ---------------------------------------------------------------------------
def build_events():
    """
    Возвращает dict pid -> список событий.
    Каждое событие: (t_start, t_end, kind, label)
    kind: 'init', 'rank', 'size', 'send', 'recv', 'final', 'printf'
    """
    events = {r: [] for r in range(SIZE)}

    # --- общий пролог: MPI_Init, MPI_Comm_rank, MPI_Comm_size ---
    for r in range(SIZE):
        t = 0.0
        events[r].append((t, t + T_MPI_INIT, "init", "MPI_Init"))
        t += T_MPI_INIT
        events[r].append((t, t + T_COMM_RANK, "rank", "MPI_Comm_rank"))
        t += T_COMM_RANK
        events[r].append((t, t + T_COMM_SIZE, "size", "MPI_Comm_size"))
        t += T_COMM_SIZE

    t_common = T_MPI_INIT + T_COMM_RANK + T_COMM_SIZE

    # -----------------------------------------------------------------
    # Логика 1.c:
    #   rank 0: цикл i = 1..size-1:
    #       заполнить a[i-1][0..9]
    #       MPI_Send(a[i-1], 10, INT, dest=i, tag=777)
    #   rank != 0:
    #       MPI_Recv(b, 10, INT, src=0, tag=777)
    #       printf
    # -----------------------------------------------------------------

    # ---- rank 0: серия из (size-1) Send'ов подряд ----
    t0 = t_common
    for i in range(1, SIZE):
        events[0].append((t0, t0 + T_SEND_SMALL, "send",
                          f"MPI_Send(→{i}, tag=777)"))
        t0 += T_SEND_SMALL
    events[0].append((t0, t0 + T_FINALIZE, "final", "MPI_Finalize"))

    # ---- остальные ранги: Recv от 0 + printf ----
    # Recv стартует сразу после пролога и ждёт, пока rank 0 до него дойдёт.
    # Момент прихода: t_common + i * T_SEND_SMALL
    # (rank 0 шлёт последовательно: 1-й Send кончается в t_common+T_SEND_SMALL,
    #  2-й — в t_common+2*T_SEND_SMALL, и т.д.)
    for r in range(1, SIZE):
        recv_start = t_common
        recv_end   = t_common + r * T_SEND_SMALL
        events[r].append((recv_start, recv_end, "recv",
                          f"MPI_Recv(←0, tag=777)"))
        t = recv_end
        events[r].append((t, t + T_PRINTF, "printf", "printf(result)"))
        t += T_PRINTF
        events[r].append((t, t + T_FINALIZE, "final", "MPI_Finalize"))

    return events


# ---------------------------------------------------------------------------
# Рисование
# ---------------------------------------------------------------------------
def draw_arrow(ax, x0, y0, x1, y1):
    """Жёлтая стрелка message между дорожками (как в Jumpshot)."""
    arrow = FancyArrowPatch(
        (x0, y0 + 0.35), (x1, y1 - 0.35),
        arrowstyle="-|>", mutation_scale=12,
        color=C_MSG, linewidth=1.4,
        connectionstyle="arc3,rad=0.15",
        zorder=5,
    )
    ax.add_patch(arrow)


def plot(events, output="jumpshot_pseudo_1c.png"):
    pids = sorted(events.keys())
    n = len(pids)
    bar_h = 0.7

    t_max = max(e[1] for evs in events.values() for e in evs)
    t_max *= 1.08

    fig_h = max(4.5, 0.9 * n + 2.5)
    fig, ax = plt.subplots(figsize=(14, fig_h))

    # ---- рисуем блоки ----
    for i, r in enumerate(pids):
        y = i
        for (t0, t1, kind, label) in events[r]:
            width = t1 - t0
            if kind == "send":
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=C_SEND, edgecolors="none", zorder=3)
            elif kind == "recv":
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=C_RECV, edgecolors="none", zorder=3)
            elif kind in ("rank", "size"):
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=C_RANK, edgecolors="black",
                               linewidth=0.8, zorder=3)
            elif kind == "final":
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=C_FINAL, edgecolors="none", zorder=3)
            elif kind == "init":
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=C_INIT, edgecolors="none", zorder=2)
            elif kind == "printf":
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=C_PRINTF, edgecolors="none", zorder=3)

    # ---- жёлтые стрелки-message ----
    # rank 0 -> i, tag=777, для i = 1..size-1
    # Берём i-й по счёту Send у rank 0 (i=1 → индекс 3+0, т.к. 3 пролога)
    sends_rank0 = [e for e in events[0] if e[2] == "send"]
    for idx, r in enumerate(range(1, SIZE)):
        send_ev = sends_rank0[idx]
        recv_ev = next(e for e in events[r] if e[2] == "recv")
        draw_arrow(ax, send_ev[0], 0, recv_ev[0], r)

    # ---- подписи слева ----
    for i, r in enumerate(pids):
        ax.text(-0.01 * t_max, i, f"Rank {r}",
                ha="right", va="center", fontsize=10, fontweight="bold")

    ax.set_yticks([])
    ax.set_xlabel("Время, с", fontsize=11)
    ax.set_xlim(0, t_max)
    ax.set_ylim(-0.7, n - 0.3)
    ax.invert_yaxis()
    ax.grid(True, axis="x", linestyle=":", alpha=0.4)
    ax.set_axisbelow(True)
    ax.set_title("Псевдо-трасса MPI 1.c (звёздная рассылка, size=6), стиль Jumpshot",
                 fontsize=13, fontweight="bold")

    # ---- Легенда ----
    handles = [
        mpatches.Patch(color=C_SEND,   label="MPI_Send"),
        mpatches.Patch(color=C_RECV,   label="MPI_Recv"),
        mpatches.Patch(facecolor=C_RANK, edgecolor="black",
                       label="MPI_Comm_rank / MPI_Comm_size"),
        mpatches.Patch(color=C_FINAL,  label="MPI_Finalize"),
        mpatches.Patch(color=C_INIT,   label="MPI_Init"),
        mpatches.Patch(color=C_PRINTF, label="printf"),
        Line2D([0], [0], color=C_MSG, lw=1.5, label="message (tag=777)"),
    ]
    ax.legend(handles=handles, loc="upper center",
              bbox_to_anchor=(0.5, -0.10), ncol=4, fontsize=9,
              frameon=True)

    plt.tight_layout()
    plt.savefig(output, dpi=150, bbox_inches="tight")
    print(f"Сохранено: {output}")


# ---------------------------------------------------------------------------
if __name__ == "__main__":
    events = build_events()
    plot(events, "jumpshot_pseudo_1c.png")
