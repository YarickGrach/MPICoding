#!/usr/bin/env python3
"""
Псевдо-трасса MPI-программы 1.c (кольцевой обмен) на size=6.
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
SIZE = 6                     # число процессов (по условию)
T_MPI_INIT   = 0.008         # инициализация MPI_Init
T_COMM_RANK  = 0.001         # MPI_Comm_rank
T_COMM_SIZE  = 0.001         # MPI_Comm_size
T_SEND_SMALL = 0.004         # передача 1 int (MPI_Send)
T_RECV_WAIT  = 0.006         # ожидание MPI_Recv
T_FINALIZE   = 0.006         # MPI_Finalize
T_PRINTF     = 0.002         # printf у rank=0

# Цвета — как в Jumpshot
C_SEND   = "#0000ff"   # синий
C_RECV   = "#00ff00"   # зелёный
C_RANK   = "#ffffff"   # белый (с чёрной рамкой)
C_SIZE   = "#ffffff"   # белый (с чёрной рамкой)
C_FINAL  = "#ff9900"   # оранжевый
C_INIT   = "#cccccc"   # серый — MPI_Init
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

    for r in range(SIZE):
        t = 0.0
        # MPI_Init
        events[r].append((t, t + T_MPI_INIT, "init", "MPI_Init"))
        t += T_MPI_INIT
        # MPI_Comm_rank
        events[r].append((t, t + T_COMM_RANK, "rank", "MPI_Comm_rank"))
        t += T_COMM_RANK
        # MPI_Comm_size
        events[r].append((t, t + T_COMM_SIZE, "size", "MPI_Comm_size"))
        t += T_COMM_SIZE
        events[r].append(("__start_loop__", t, None, None))  # маркер

    # ---- Основная логика из 1.c ----
    # rank 0: Send -> 1 (tag 111), затем Recv <- size-1 (tag 777)
    # rank i (1..size-2): Recv <- i-1 (tag 111), n+=1, Send -> i+1 (tag 111)
    # rank size-1: Recv <- size-2 (tag 111), n+=1, Send -> 0 (tag 777)

    # Начальный момент после инициализации
    t_common = T_MPI_INIT + T_COMM_RANK + T_COMM_SIZE

    # ---- rank 0 ----
    t0 = t_common
    events[0].append((t0, t0 + T_SEND_SMALL, "send", "MPI_Send(→1, tag=111)"))
    t0_send0 = t0
    t0 += T_SEND_SMALL
    # Ждём ответ от последнего процесса
    t0_recv_start = t0
    events[0].append((t0, t0 + T_RECV_WAIT, "recv",
                      "MPI_Recv(←5, tag=777)"))
    t0 += T_RECV_WAIT
    # printf
    events[0].append((t0, t0 + T_PRINTF, "printf", "printf(result)"))
    t0 += T_PRINTF
    events[0].append((t0, t0 + T_FINALIZE, "final", "MPI_Finalize"))

    # ---- rank 1..4 ----
    for r in range(1, SIZE - 1):
        t = t_common
        # Recv от предыдущего (начинается сразу, ждёт пока придёт)
        # Реально Recv блокируется, пока Send(r-1) не дойдёт.
        # Моделируем: Recv стартует сразу после init,
        # но завершается, когда приходит сообщение.
        recv_start = t
        # Момент прихода: (r-1)-й отправил через T_SEND_SMALL после своего старта
        # Для r=1: rank0 отправил в t_common
        # Для r>=2: rank(r-1) отправил после своего Recv, т.е. последовательно
        recv_end = t_common + r * (T_SEND_SMALL + 0.001)
        events[r].append((recv_start, recv_end, "recv",
                          f"MPI_Recv(←{r-1}, tag=111)"))
        t = recv_end
        # n += 1 — мгновенно, не рисуем
        # Send -> r+1 (tag 111)
        events[r].append((t, t + T_SEND_SMALL, "send",
                          f"MPI_Send(→{r+1}, tag=111)"))
        t += T_SEND_SMALL
        events[r].append((t, t + T_FINALIZE, "final", "MPI_Finalize"))

    # ---- rank size-1 ----
    r = SIZE - 1
    t = t_common
    recv_start = t
    recv_end = t_common + r * (T_SEND_SMALL + 0.001)
    events[r].append((recv_start, recv_end, "recv",
                      f"MPI_Recv(←{r-1}, tag=111)"))
    t = recv_end
    # n += 1 — мгновенно
    # Send -> 0 (tag 777)
    events[r].append((t, t + T_SEND_SMALL, "send",
                      "MPI_Send(→0, tag=777)"))
    t += T_SEND_SMALL
    events[r].append((t, t + T_FINALIZE, "final", "MPI_Finalize"))

    # Убираем служебные маркеры
    for r in events:
        events[r] = [e for e in events[r] if e[0] != "__start_loop__"]

    return events


# ---------------------------------------------------------------------------
# Рисование
# ---------------------------------------------------------------------------
def plot(events, output="jumpshot_pseudo.png"):
    pids = sorted(events.keys())
    n = len(pids)
    bar_h = 0.7

    # t_max — максимум t_end по всем событиям
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
                color = C_SEND
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=color, edgecolors="none", zorder=3)
            elif kind == "recv":
                color = C_RECV
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=color, edgecolors="none", zorder=3)
            elif kind in ("rank", "size"):
                color = C_RANK
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=color, edgecolors="black",
                               linewidth=0.8, zorder=3)
            elif kind == "final":
                color = C_FINAL
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=color, edgecolors="none", zorder=3)
            elif kind == "init":
                color = C_INIT
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=color, edgecolors="none", zorder=2)
            elif kind == "printf":
                color = "#ffcc99"
                ax.broken_barh([(t0, width)], (y - bar_h / 2, bar_h),
                               facecolors=color, edgecolors="none", zorder=3)

    # ---- жёлтые стрелки-сообщения ----
    # rank 0 -> 1 (tag 111)
    draw_arrow(ax, events[0][2][0], 0, events[1][2][0], 1)
    # rank r -> r+1 (tag 111) для r=1..4
    for r in range(1, SIZE - 1):
        # send в этом rank
        send_ev = next(e for e in events[r] if e[2] == "send")
        # recv в следующем
        recv_ev = next(e for e in events[r + 1] if e[2] == "recv")
        draw_arrow(ax, send_ev[0], r, recv_ev[0], r + 1)
    # rank 5 -> 0 (tag 777)
    send_ev = next(e for e in events[SIZE - 1] if e[2] == "send")
    recv_ev = next(e for e in events[0] if e[2] == "recv")
    draw_arrow(ax, send_ev[0], SIZE - 1, recv_ev[0], 0)

    # ---- подписи слева: Rank ----
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
    ax.set_title("Псевдо-трасса MPI 1.c (size=6), стиль Jumpshot",
                 fontsize=13, fontweight="bold")

    # ---- Легенда ----
    handles = [
        mpatches.Patch(color=C_SEND,   label="MPI_Send"),
        mpatches.Patch(color=C_RECV,   label="MPI_Recv"),
        mpatches.Patch(facecolor=C_RANK, edgecolor="black",
                       label="MPI_Comm_rank / MPI_Comm_size"),
        mpatches.Patch(color=C_FINAL,  label="MPI_Finalize"),
        mpatches.Patch(color=C_INIT,   label="MPI_Init"),
        mpatches.Patch(color="#ffcc99", label="printf"),
        Line2D([0], [0], color=C_MSG, lw=1.5, label="message (tag)"),
    ]
    ax.legend(handles=handles, loc="upper center",
              bbox_to_anchor=(0.5, -0.10), ncol=4, fontsize=9,
              frameon=True)

    plt.tight_layout()
    plt.savefig(output, dpi=150, bbox_inches="tight")
    print(f"Сохранено: {output}")


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


# ---------------------------------------------------------------------------
if __name__ == "__main__":
    events = build_events()
    plot(events, "jumpshot_pseudo.png")
